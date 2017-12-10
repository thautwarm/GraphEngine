using System;
using System.Collections.Generic;
using System.Threading.Tasks;


namespace Trinity.DynamicCluster.Persistency
{
    using RawData = ValueTuple<long, ushort, byte[]>;

    public interface IPersister
    {
        bool IsCacheFilledUp();
        bool DumpToBlobStorageServer(Guid guid, List<byte[]> srcs);
        void RegisterData(Guid guid, IEnumerable<RawData> rawDatas);
        void Dump();


        Task Start();
    }
}