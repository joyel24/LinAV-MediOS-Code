.text

.globl nb_wav_chunk
nb_wav_chunk: .long 0xB

.globl mp3Buff
mp3Buff: .word mp3BuffData

.globl wav_chunk_size
wav_chunk_size: .long wav_chunk_size_array
wav_chunk_size_array:
    .long 0x1
    .long 0x7E
    .long 0x14A
    .long 0x52
    .long 0x46
    .long 0x4E
    .long 0x102
    .long 0x36
    .long 0x76
    .long 0x1A16
    .long 0x10DA

.globl wav_chunks
wav_chunks: .long wav_chunks_array
wav_chunks_array:
    .long chunk_0
    .long chunk_1
    .long chunk_2
    .long chunk_3
    .long chunk_4
    .long chunk_5
    .long chunk_6
    .long chunk_7
    .long chunk_8
    .long chunk_9
    .long chunk_A
    

chunk_0:
.incbin "./wav_chunk/chunk_0"
chunk_1:
.incbin "./wav_chunk/chunk_1"
chunk_2:
.incbin "./wav_chunk/chunk_2"
chunk_3:
.incbin "./wav_chunk/chunk_3"
chunk_4:
.incbin "./wav_chunk/chunk_4"
chunk_5:
.incbin "./wav_chunk/chunk_5"
chunk_6:
.incbin "./wav_chunk/chunk_6"
chunk_7:
.incbin "./wav_chunk/chunk_7"
chunk_8:
.incbin "./wav_chunk/chunk_8"
chunk_9:
.incbin "./wav_chunk/chunk_9"
chunk_A:
.incbin "./wav_chunk/chunk_A"

mp3BuffData:
.incbin "./wav_chunk/pcm_file"