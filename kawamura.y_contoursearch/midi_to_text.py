import sys
import pretty_midi
import csv

if len(sys.argv) < 2:
    print("No argument")
    sys.exit()

# MIDIファイルのロード
midi_data = pretty_midi.PrettyMIDI(sys.argv[1])

# トラック別で取得
midi_tracks = midi_data.instruments

# トラック1のノートを取得
notes = midi_tracks[0].notes

for note in notes:
    # Note(noteon_time, noteoff_time, note_number, velocity)
    print(note) 

#　ノートナンバーのリストを作成
note_number_list = []
for note in notes:
    note_number_list.append(note.pitch)

##print(note_number_list)

# 辞書の作成
pitch = [127, 126, 125, 124, 123, 122, 121, 120, 119, 
         118, 117, 116, 115, 114, 113, 112, 111, 110, 
         109, 108, 107, 106, 105, 104, 103, 102, 101,
         100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 
         89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 
         77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 
         65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 
         53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 
         41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 
         17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 
         3, 2, 1, 0]

scale = ['G9', 'F#9', 'F9', 'E9', 'D#9', 'D9', 'C#9', 
         'C9', 'B8', 'A#8', 'A8', 'G#8', 'G8', 'F#8', 
         'F8', 'E8', 'D#8', 'D8', 'C#8', 'C8', 'B7', 
         'A#7', 'A7', 'G#7', 'G7', 'F#7', 'F7', 'E7', 
         'D#7', 'D7', 'C#7', 'C7', 'B6', 'A#6', 'A6', 
         'G#6', 'G6', 'F#6', 'F6', 'E6', 'D#6', 'D6', 
         'C#6', 'C6', 'B5', 'A#5', 'A5', 'G#5', 'G5', 
         'F#5', 'F5', 'E5', 'D#5', 'D5', 'C#5', 'C5', 
         'B4', 'A#4', 'A4', 'G#4', 'G4', 'F#4', 'F4',
         'E4', 'D#4', 'D4', 'C#4', 'C4', 'B3', 'A#3', 
         'A3', 'G#3', 'G3', 'F#3', 'F3', 'E3', 'D#3', 
         'D3', 'C#3', 'C3', 'B2', 'A#2', 'A2', 'G#2', 
         'G2', 'F#2', 'F2', 'E2', 'D#2', 'D2', 'C#2', 
         'C2', 'B1', 'A#1', 'A1', 'G#1', 'G1', 'F#1',
         'F1', 'E1', 'D#1', 'D1', 'C#1', 'C1', 'B0',
         'A#0', 'A0', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
         'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
         'N', 'N', 'N', 'N', 'N']

d = dict(zip(pitch, scale))

#新規ファイル作成
with open("midi_text.txt","w", newline = "") as f:
    w = csv.writer(f)
    for key in note_number_list:
        value = d[key]
        w.writerow([value])
    #1行ずつ書き込み
    ##for note in notes:
    #    w.writerow(note_number_list)
    #    w.writerow([note.pitch])

def create_table(pattern):
    table = [0 for _ in range(len(pattern))]
    j = 0
    for i in range(1, len(pattern)):
        if pattern[i] == pattern[j]:
            j += 1
            table[i] = j
        else:
            table[i] = j
            j = 0
    return table

def kmp_search(string, pattern):
    table = create_table(pattern)
    i = j = 0
    while i < len(string) and j < len(pattern):
        if string[i] == pattern[j]:
            i += 1
            j += 1
        elif j == 0:
            i += 1
        else:
            j = table[j]
 
    if j == len(pattern):
        return i - j
    else:
        return None
 
with open("midi_text.txt", "r") as f:
    string = f.read()

#pattern = sys.argv[2]
pattern = "B4\nA4\nG4\nE4\nG4\nB4\nA4"

index = kmp_search(string, pattern)
if index:
    print("Pattern match Suceed.")
else:
    print("No pattern matched.")
