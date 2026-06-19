 INDEXED FILE DATABASE ENGINE      (STUDENT RMS) 

A high-performance, disk-based Student Record Management System (RMS) built in C++.
Unlike traditional file-based systems that rely on full file scans or rewriting entire datasets, this engine leverages in-memory indexing and in-place binary updates to deliver efficient and scalable performance as data grows.




ARCHITECTURE OVERVIEW

This system is designed as a storage engine that treats disk data as structured binary records rather than plain text.
By utilizing fixed-width structures, the engine can calculate exact byte offsets for any record in the database.


1. RAM-BASED OFFSET INDEX (O(log n) Access)

At startup, the system builds an in-memory index to map logical IDs to physical locations. This ensures that the engine does not scan the physical file to locate data.

i) Data Structure:
std::map<int, std::streampos>

ii) Mapping:
Student ID → Byte Offset in File

iii) Implementation Logic:
- The file is scanned once during initialization to identify record boundaries and populate the map.
- Each active record’s position (offset) is stored in the map.
- Lookup operations use the map to find the offset and directly jump to the correct file location using seekg().
  
iv)  Performance Result:
- Eliminates linear scans (O(n)).
- Enables near-instant record access regardless of the number of records



2. SOFT DELETION (O(1) Delete)

Instead of physically removing data, which would require shifting all subsequent records on disk, the engine uses a soft-delete strategy.

i) Mechanism:
- A boolean flag isDeleted is set to true within the binary struct.
- The record’s ID is removed from the in-memory index map.
  
ii) Advantages:
- Avoids costly file shifting and heavy disk I/O.
- Provides constant-time deletion operations.
- Maintains data for possible recovery or auditing.



3. IN-PLACE BINARY UPDATES

Updates are performed directly on the disk without rewriting the entire file.

i) Mechanism:
- The engine locates the record using the RAM index.
- Only the required fields (e.g., marks) are modified.
- The updated struct is written back to the same byte location.
  
ii) Performance Result:
- Reduced disk I/O overhead.
- Consistent update performance independent of database size.



4. DATABASE COMPACTION (Storage Optimization)

Over time, soft-deleted records occupy unnecessary disk space. The compaction utility optimizes storage.

i) Process:
- A temporary binary file is created.
- Only active records (from the index) are written to the new file.
- The original file is replaced by the optimized file.
- The index is rebuilt to reflect updated offsets.
  
ii) Advantages:
- Reclaims unused disk space.
- Improves long-term read/write performance.


 KEY DESIGN PRINCIPLES

i) Binary Data Integrity
- Fixed-width character arrays are used instead of dynamic strings.
- Each record occupies the same number of bytes.
- Enables precise offset calculation for direct access.
  
ii) Memory Efficiency
- Only ID and file offset are stored in RAM.
- Actual data remains on disk until accessed.
- Allows handling of large datasets with minimal memory usage.



 CORE CONCEPTS DEMONSTRATED

- Binary File Handling using fstream
- Direct Access Storage (Random Access I/O)
- In-Memory Indexing using STL containers
- Fixed-Width Record Design
- Soft Deletion Strategy
- Storage Optimization through Compaction


SUMMARY

This project demonstrates low-level storage optimization techniques inspired by production-grade database engines.

By combining binary storage, indexing, and direct file access, it provides a scalable and efficient alternative to traditional text-based record management systems.


 FUTURE SCOPE

Phase 2: Multi-Threaded Concurrency (Isolation & Integrity)

 The current engine handles single-threaded operations perfectly but will experience data corruption under concurrent client loads.
- Reader-Writer Locks (std::shared_mutex): Implement a shared-read, exclusive-write locking mechanism. This allows thousands of concurrent clients to execute getStudent() simultaneously (O(log n)shared access) while strictly blocking the engine during an addStudent() or updateMarks() write barrier.
- Thread-Safe Index Mutations: Ensure that memory allocation changes inside the primary RAM index map are atomic during concurrent deletions and insertions

Phase 3: Disk-Aware Scaling (Breaking the RAM Barrier)

Currently, keeping the entire primary index map in volatile memory creates an O(n) memory footprint bottleneck. At 100 million records, system RAM will exhaust.
- Disk-Based B+ Tree Indexing: Replace std::map with a custom disk-backed B+ Tree structure. By storing internal routing nodes in memory and leaf data node pointers on disk blocks, the engine can scale to billions of records while keeping RAM usage completely flat and bounded.
- Write-Ahead Logging (WAL) & Durability: Introduce a sequential append-only log file. Every write operation will be recorded to the WAL before mutating the main database file, ensuring complete data recovery and ACID compliance in the event of an abrupt system power failure or crash.

Phase 4: Decentralized Architecture (TELEMETRIC DBaaS)

Decouple the storage sub-system entirely from the local terminal interface to expose it as a high-throughput network or local daemon service.
- Low-Latency IPC Bridge via Named Pipes: Build a multi-process architecture where a high-throughput Node.js/Express API gateway communicates with the native C++ binary engine across an OS-level Named Pipe (FIFO), eliminating TCP/IP network protocol stack overhead for localized execution.
- Real-Time Telemetric Analytics Dashboard: Mount a frontend UI tracking database storage health metrics (compaction cycles, raw fragmentation ratios, live query throughput, and read/write latencies measured down to the sub-millisecond).




  


  
