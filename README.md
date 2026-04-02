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

To evolve this project into a production-grade storage engine, the next phase of development involves moving toward a multi-threaded, disk-aware architecture.

* Concurrency & Multithreading: Implementing `std::mutex` and reader-writer locks to allow multiple threads to read data simultaneously while maintaining strict data integrity during writes.
* Disk-Based B-Tree Indexing: Moving from an in-memory `std::map` to a disk-based B-Tree structure. This will allow the index itself to live on the disk, enabling the engine to scale to millions of records without exhausting system RAM.




  


  
