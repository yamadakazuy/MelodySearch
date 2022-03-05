from os import supports_follow_symlinks
import sys
from typing import NoReturn
import pretty_midi
import csv
import os


if len(sys.argv) < 2:
    print("No argument")
    sys.exit()

# MIDIファイルを概形ファイルへ変換
def midi_to_abs(file_name):
    # すでに概形文字列化されてあれば, 処理を省略
    if os.path.isfile(file_name + "_abs.txt"):
        return file_name + "_abs.txt"
    
    # MIDIファイルのロード
    try:   
        midi_data = pretty_midi.PrettyMIDI(file_name)
    except IOError:
        return

    # トラック別で取得
    midi_tracks = midi_data.instruments

    # トラック1のノートを取得
    try:
        notes = midi_tracks[0].notes
    except IndexError:
        return
    
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
            note_number_fluc.append("=")
        if (0 < n1[i] - note_number_list[i] <= 2):
            note_number_fluc.append("#")
        if (n1[i] - note_number_list[i] > 2):
            note_number_fluc.append("+")
        if (0 > n1[i] - note_number_list[i] >= -2):
            note_number_fluc.append("b")
        if (n1[i] - note_number_list[i] < -2):
            note_number_fluc.append("-")
    note_number_fluc.pop()

    #MIDI概形のテキストファイルを作成
    with open(file_name + "_abs.txt", "w", newline = "") as f:
        w = csv.writer(f)
        w.writerow(note_number_fluc)

    return file_name+"_abs.txt"


#ペース（速い・中間・遅い）の取得
def get_tempo(file_name):
    # MIDIファイルのロード
    midi_data = pretty_midi.PrettyMIDI(file_name)
    # テンポの取得
    tempo = midi_data.get_tempo_changes()
    bpm = max(tempo[1])

    if bpm < 90:
        pace = 'slow'
    elif bpm >= 90 and bpm < 140:
        pace = 'middle'
    elif bpm >= 140:
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
def kmp_search(text, pattern):
    table = create_table(pattern)
    i = j = 0
    while i < len(text) and j < len(pattern):
        if text[i] == pattern[j]:
            i += 1
            j += 1
        #曖昧照合
        elif text[i] == '#' and pattern[j] == '+':
            j += 1 
            i += 1
        elif text[i] == 'b' and pattern[j] == '-':
            j += 1
            i += 1
        elif j == 0:
            i += 1
        else:
            j = table[j]
 
    if j == len(pattern):
        return i - j
    else:
        return None

if __name__ == '__main__':
    # ディレクトリの読み込み
    target_dir = sys.argv[1]
    # 探すメロディパタンの読み込み
    pattern = sys.argv[2]
    # 探すメロディパタンのテンポの読み込み
    user_pace = sys.argv[3]
    
    # マッチしたファイルリストの作成
    file_list = []   #テンポを考慮したファイルリスト
    file_list0 = []  #テンポを考慮しないファイルリスト

    for filename in os.listdir(target_dir):
        path_in = os.path.join(target_dir, filename)
        if not (os.path.isfile(path_in) and path_in.endswith('.mid')):
            continue
        #print(path_in)
        path_out = midi_to_abs(path_in)
        #print(path_out)

        try:
            with open(path_out, "r") as f:
                text = f.read()
        except:
            continue
        
        path_pace = get_tempo(path_in)
        #print(path_pace)

        index = kmp_search(text, pattern)
        #index = Horspool_search(text, pattern)

        if index != None and path_pace == user_pace:
            file_list.append(filename)
        if index != None:
            file_list0.append(filename)
    
    # file_list(テンポを考慮したファイルリスト)
    print(file_list, len(file_list))
    print('\n')
    # file_list0(テンポを考慮しないファイルリスト)
    print(file_list0, len(file_list0))
