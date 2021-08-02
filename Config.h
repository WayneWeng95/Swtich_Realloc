#define ENABLE_PREDICTION 0
#define ENABLE_UNSHRINK_NOW 0 //need enable prediction first
#define MMAP_IN_SMALLSIZE 0
#define ENABLE_HUGLETLB 0 //need additional checking before enable this

#define SWITCH_POINT 128

#define AGGRESIVE 0

// #define HUGE_PAGE_SIZE 2 * 1024 * 1024 //2048KB as system default

//need to check the configuation first.  using: cat /proc/filesystems
//When the configure available, check the
//number of huge table in the pool by    using: cat /proc/sys/vm/nr_hugepages
//Adding more hugepage by:   echo 20 > /proc/sys/vm/nr_hugepages
//https://www.kernel.org/doc/html/latest/admin-guide/mm/hugetlbpage.html
//https://github.com/libhugetlbfs/libhugetlbfs

// Users who wish to use hugetlb memory via shared memory segment should be members of a supplementary group and system admin needs to configure
// that gid into /proc/sys/vm/hugetlb_shm_group. It is possible for same or different applications to use any combination of mmaps and shm* calls,
// though the mount of filesystem will be required for using mmap calls without MAP_HUGETLB.

// Syscalls that operate on memory backed by hugetlb pages only have their
// lengths aligned to the native page size of the processor; they will normally fail with errno set to
// EINVAL or exclude hugetlb pages that extend beyond the length if not hugepage aligned. For example, munmap(2)
// will fail if memory is backed by a hugetlb page and the length is smaller than the hugepage size.

//!3377
//!3378