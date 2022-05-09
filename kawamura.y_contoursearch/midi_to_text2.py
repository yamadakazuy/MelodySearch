from os import supports_follow_symlinks
import sys
import pretty_midi
import csv
import os
import glob

#../smf B4A4G4E4G4B4A4 slow

if len(sys.argv) < 2:
    print("No argument")
    sys.exit()


# MIDIファイルを概形ファイルへ変換
def midi_to_abs(file_name):
    try:
        # MIDIファイルのロード
        midi_data = pretty_midi.PrettyMIDI(file_name)
    except IOError as ex:
        print(ex, filename)
    # トラック別で取得
    midi_tracks = midi_data.instruments
    # トラック1のノートを取得
    notes = midi_tracks[0].notes
    #　ノートナンバーのリストを作成
    note_number_list = []
    for note in notes:
        note_number_list.append(note.pitch)

    #左に1つずらしたノートナンバーのリストの作成
    n1 = note_number_list[1:] + note_number_list[:1]

    #数値を概形表現へ変換したリストの作成
    note_number_fluc = []
    for i in range(0,len(note_number_list)):
        if (n1[i] == note_number_list[i]):
            note_number_fluc.append("→")
        if (n1[i] == note_number_list[i]+1):
            note_number_fluc.append("#")
        if (n1[i] > note_number_list[i]+1):
            note_number_fluc.append("↑")
        if (n1[i] == note_number_list[i]-1):
            note_number_fluc.append("b")
        if (n1[i] < note_number_list[i]-1):
            note_number_fluc.append("↓")
    note_number_fluc.pop()

    #MIDI概形のテキストファイルを作成
    with open(file_name + "_abs.txt", "w", newline = "") as f:
        w = csv.writer(f)
        w.writerow(note_number_fluc)
    
    return file_name + "_abs.txt"

#ペース（速い・中間・遅い）の取得
def get_tempo(file_name):
    # MIDIファイルのロード
    midi_data = pretty_midi.PrettyMIDI(file_name)
    # テンポの取得
    tempo = midi_data.get_tempo_changes()
    print(tempo[1])
    if min(tempo[1]) < 90:
        pace = 'slow'
    elif max(tempo[1]) >= 90 and min(tempo[1]) < 140:
        pace = 'middle'
    elif max(tempo[1]) >= 140:
        pace = 'fast'   

    return pace


#ずらし表の作成
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


#KMP法の実装
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


if __name__ == '__main__':
    target_dir = sys.argv[1]
    # 探すメロディパタンの読み込み
    pattern = sys.argv[2]
    # 探すメロディパタンのテンポの読み込み
    user_pace = sys.argv[3]
    # マッチしたファイルリストの作成
    file_list = []

    for filename in os.listdir(target_dir):
        if not filename.endswith('.mid'):
            continue
        path_in = os.path.join(target_dir, filename)
        path_out = midi_to_abs(path_in)
        print(path_out)
        with open(path_out, "r") as f:
            string = f.read()
        
        path_pace = get_tempo(path_in)

        index = kmp_search(string, pattern)

        if index and path_pace == user_pace:
            file_list.append(filename)
    else:
        print("No files.")
    print(file_list)