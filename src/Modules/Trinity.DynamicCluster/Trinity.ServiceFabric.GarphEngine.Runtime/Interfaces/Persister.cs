using System;
using System.Collections.Generic;
using System.Fabric;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Trinity.DynamicCluster;
using Trinity.DynamicCluster.Persistency;

namespace Trinity.ServiceFabric.GarphEngine.Infrastructure.Interfaces
{
    using RawData = ValueTuple<long, ushort, byte[]>;

    public class Persister : IPersister
    {
        protected int MaxTolerate = 3;
        private int BoundarySize;
        private int ReferenceAmount;

        private Dictionary<Guid, List<byte[]>> m_cache;
        private PersistencyInfo m_pstinfo;
        private ReplicaInformation m_replica;
        private ISerializer m_serializer;

        public Func<Task> End = null;


        public Persister(ReplicaInformation replicaInfo, PersistencyInfo persistencyInfo, ISerializer serializer)
        {
            m_replica = replicaInfo;
            m_pstinfo = persistencyInfo;
            m_serializer = serializer;
            ReferenceAmount = 0;
        }

        public void RegisterData(Guid guid, IEnumerable<RawData> rawDatas)
        {
            if (m_cache.ContainsKey(guid))
            {
                m_cache[guid].AddRange(rawDatas.Select(m_serializer.Apply));
                ReferenceAmount += m_serializer.CountAndRefresh();
            }
            else
            {
                m_cache[guid] = rawDatas.Select(m_serializer.Apply).ToList();
                ReferenceAmount += m_serializer.CountAndRefresh();
            }
        }


        public bool IsCacheFilledUp()
        {
            // TODO : Use `ReferenceAmount`, `BoundarySize`, `m_pstinfo` and `m_pstinfo` to decide if do dumping right now. 
            throw new NotImplementedException();
        }


        public bool DumpToBlobStorageServer(Guid guid, List<byte[]> srcs)
        {
            // TODO: Connect to the remote server again and dump `srcs` to the remote with version `guid`.
            throw new NotImplementedException();
        }


        public void Dump()
        {
            var status = Enumerable.Range(0, m_cache.Count).Select(_ => false).ToArray();


            void DumpEach(object state)
            {
                var (loc, dir, srcs) = state as (int, Guid, List<byte[]>)? ?? (0, Guid.Empty, null);

                if (srcs == null || dir.Equals(Guid.Empty)) return;

                var attemptCount = 0;
                retry:

                if (DumpToBlobStorageServer(dir, srcs))
                {
                    // Succeed
                    goto end;
                }
                // Retry
                attemptCount++;
                if (attemptCount < MaxTolerate) goto retry;
                
                // Fail and write to the console.
                Console.WriteLine($"Source(version={dir})has failed in dumping to the remote.");

                end:
                status[loc] = true;
            }

            m_cache.ForEach( (i, kv) => ThreadPool.QueueUserWorkItem(DumpEach, (i, kv.Key, kv.Value)));
            while (!status.All(_ => _))
            {
                Thread.Sleep(1000);
            }
        }

        public async Task Start()
        {
            var readyToOffine = false;
            while (!readyToOffine)
            {
                Thread.Sleep(1000);
                if (IsCacheFilledUp())
                {
                    readyToOffine = true;

                    async Task ShiftTask()
                    {
                        // TODO: Do something before dumping(offline at the same time), such as transfer this replica to a new one.
                        // await ...
                    }


                    if (End != null) await End();
                    await ShiftTask();
                    Dump();
                    m_cache.Clear();
                    ReferenceAmount = 0;
                }
            }
        }
    }
}