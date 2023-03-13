#include <stdio.h>
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <algorithm>

#include "../src/daemon/common.h"

#define ALIGNMENT 64

void* aligned_alloc(size_t alignment, size_t size) {
    if (alignment % sizeof(void*) != 0) {
        errno = EINVAL;
        return NULL;
    }
    if (size == 0) {
        size = 1;
    }
    void* mem;
    if (posix_memalign(&mem, alignment, size) != 0) {
        return NULL;
    }
    return mem;
}

void aligned_free(void* aligned_mem) {
    free(aligned_mem);
}


void checkAlignment(char const * debugMessage, void* ptr)
{
	printf("Checking: %-20s\t\t", debugMessage);
	uint64_t currentAlignment = (uint64_t)ptr % ALIGNMENT;
	if(currentAlignment != 0)
	{
		printf("Failed\n");
		printf("Current alignment: %llu\n\n", (unsigned long long)currentAlignment);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Passed\n");
	}
}

int main()
{
    printf("Testing alignment\n\n");

    PCMDaemon::SharedPCMState* pcmState = (PCMDaemon::SharedPCMState*)aligned_alloc(ALIGNMENT, sizeof(PCMDaemon::SharedPCMState));

    if (pcmState == nullptr)
    {
        printf("Memory allocation failed\n\n");
        exit(EXIT_FAILURE);
    }

    std::fill((char*)pcmState, ((char*)pcmState) + sizeof(PCMDaemon::SharedPCMState), 0);

    checkAlignment("pcmState", pcmState);

    checkAlignment("pcm", &pcmState->pcm);

    checkAlignment("pcm core", &pcmState->pcm.core);
    checkAlignment("pcm memory", &pcmState->pcm.memory);
    checkAlignment("pcm qpi", &pcmState->pcm.qpi);

    for(uint32_t i(0); i < MAX_CPU_CORES; ++i)
    {
    	checkAlignment("pcm core cores", &pcmState->pcm.core.cores[i]);
    }

    checkAlignment("pcm core energyUsed", &pcmState->pcm.core.energyUsedBySockets);

    for(uint32_t i(0); i < MAX_SOCKETS; ++i)
    {
    	checkAlignment("pcm memory sockets", &pcmState->pcm.memory.sockets[i]);
    }

    for(uint32_t i(0); i < MAX_SOCKETS; ++i)
    {
    	checkAlignment("pcm qpi incoming", &pcmState->pcm.qpi.incoming[i]);
    }
    
    for(uint32_t i(0); i < MAX_SOCKETS; ++i)
    {
    	for(uint32_t j(0); j < QPI_MAX_LINKS; ++j)
    	{
    		checkAlignment("pcm qpi incoming links", &pcmState->pcm.qpi.incoming[i].links[j]);
    	}
    }

    for(uint32_t i(0); i < MAX_SOCKETS; ++i)
    {
    	checkAlignment("pcm qpi outgoing", &pcmState->pcm.qpi.outgoing[i]);
    }

    for(uint32_t i(0); i < MAX_SOCKETS; ++i)
    {
    	for(uint32_t j(0); j < QPI_MAX_LINKS; ++j)
    	{
    		checkAlignment("pcm qpi outgoing links", &pcmState->pcm.qpi.outgoing[i].links[j]);
    	}
    }

    free(pcmState);

    printf("\n------ All passed ------\n\n");

    return EXIT_SUCCESS;
}
