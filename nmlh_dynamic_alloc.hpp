#include <cstddef>
#include <cstdint>
#include <unistd.h>

namespace nmlh{
    const size_t KB = 1024;
    const size_t MB = 1024 * KB;
    const size_t GB = 1024 * MB;
    const size_t TB = 1024 * GB;

    typedef unsigned char byte_t;
    typedef uintptr_t word_t;

    // The blocks are linked together
    struct memory_block{
        size_t block_header = 0;
        memory_block* next_block = nullptr;

        /**
         * @brief Returns if the memory block is used
         */
        inline bool used() const {return block_header & 1;}

        /**
         * @brief Returns the size of the memory block data
         */
        inline size_t size() const {return block_header & ~(sizeof(word_t) - 1);}

        /**
         * @brief Returns the location of this memory block's data
         */
        inline byte_t* data() const {return (byte_t*)(this + 1);}
    };

    // The sectors are linked together.
    struct page_sector{
        size_t sector_size = 0;
        page_sector* next_sector = nullptr;
        memory_block* top_memory_block = nullptr;
        byte_t* brk = (byte_t*)this + sizeof(page_sector);

        void* sbrk(size_t inc);
        inline memory_block* first_block() const {return (memory_block*)(this + 1);}
    };

    size_t align(size_t size){return (size + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);}

    class allocator{
        private:
            memory_block* request(page_sector* desired_sector, size_t size);

        public:
            allocator();
            ~allocator();

            template<class C>
            C* alloc(size_t count);

            /*
            the library has a specialization for the void-type

            template<>
            void* alloc<void>(size_t size);
            */
           
            void free(void* free_ptr);

        private:
            page_sector* first_sector = nullptr;
            page_sector* top_sector = nullptr;
            size_t page_size = getpagesize();
            size_t page_sector_default_size = page_size * 256;
    };
}