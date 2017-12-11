using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Trinity.DynamicCluster.Persistency
{
    [Flags]
    public enum PersistentStorageMode : int
    {
        /* Locality */
        Local,
        Replication,
        External,
        /* Medium */
        // TODO: Review this collection and complete the list.    
        VolatileMemory,
        NonVolatileMemory,
        MechanicalDrive,
        SolidStateDrive,
        RowStore,
        /* Accessibility */
        FileSystem, // Azure Blob Storage, HDFS, etc.
        RowStore,
        AddressSpace, // Memory, buffer, etc.
    }
}
