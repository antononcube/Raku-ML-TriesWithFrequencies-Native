name = libTriesWithFrequencies.so

all: /home/vscode/Raku-ML-TriesWithFrequencies-Native/resources/libraries/libTriesWithFrequencies.so
clean:
	rm /home/vscode/Raku-ML-TriesWithFrequencies-Native/resources/libraries/libTriesWithFrequencies.so

/home/vscode/Raku-ML-TriesWithFrequencies-Native/resources/libraries/libTriesWithFrequencies.so: TriesWithFrequencies.c
ifeq ($(shell uname), Darwin)
	gcc -shared -fPIC -fwrapv -std=gnu99 -Wextra -Wall -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces -Werror=pointer-arith -O3 -DNDEBUG  -D_REENTRANT -D_FILE_OFFSET_BITS=64 -fPIC -DMVM_HEAPSNAPSHOT_FORMAT=3 -D_GNU_SOURCE TriesWithFrequencies.c -o /home/vscode/Raku-ML-TriesWithFrequencies-Native/resources/libraries/libTriesWithFrequencies.so -framework Accelerate
else
	gcc -shared -fPIC -fwrapv -std=gnu99 -Wextra -Wall -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces -Werror=pointer-arith -O3 -DNDEBUG  -D_REENTRANT -D_FILE_OFFSET_BITS=64 -fPIC -DMVM_HEAPSNAPSHOT_FORMAT=3 -D_GNU_SOURCE TriesWithFrequencies.c -o /home/vscode/Raku-ML-TriesWithFrequencies-Native/resources/libraries/libTriesWithFrequencies.so
endif