'''
Created on 2022/03/05

@author: Sin Shimozono
'''

import sys, os, pretty_midi

MELODIC_CONTOUR_EXT = 'mct'

# MIDIファイルを概形ファイルへ変換
def smf2mct(targert_dir, mid_file):
    # MIDIファイルのロード
    try:   
        midi = pretty_midi.PrettyMIDI(os.path.join(target_dir, mid_file))
    except IOError:
        return False

    # ヘッダ記載のテンポの取得
    # tempo_changes = midi.get_tempo_changes()
    # print(tempo_changes)

    # トラック別で取得
    tracks = midi.instruments
    if len(tracks) == 0 :
        return False
    
    # トラック1のノートを取得
    score = []
    for track in tracks:
        part = []
        for note in track.notes:
            part.append((note.start, note.pitch))
        if len(part) > 0 :
            seq = []
            for n in sorted(part):
                if len(seq) == 0 :
                    seq.append([n[0], n[1]])
                elif seq[-1][0] < n[0] :
                    # 前のボイスのクローズ処理
                    v = seq.pop()
                    seq.append([v[0]] + sorted(v[1:], reverse=True))
                    #
                    seq.append([n[0], n[1]])
                elif seq[-1][0] == n[0] :
                    seq[-1].append(n[1])
                else:
                    raise ValueError('reflux!')
            score.append(seq)
    
    #MIDI概形のテキストファイルを作成
    mct_file = '.'.join(mid_file.split('.')[:-1]) + '.mct'
    print(os.path.join(target_dir, mct_file))
    with open(os.path.join(target_dir, mct_file), "w") as f:
        # f.write(','.join([str(t) for t in tempo_changes]))
        # f.write('\n')
        for part in score:
            prev = None
            for voice in part:
                if prev == None:
                    pass
                else:
                    itv = voice[1] - prev
                    if itv == 0 :
                        f.write('=')
                    elif itv < 0 :
                        if itv < -2 :
                            f.write('-')
                        else:
                            f.write('b')
                    else:
                        if itv > 2 :
                            f.write('+')
                        else:
                            f.write('#')
                prev = voice[1]
            f.write('\n')

    return True


if __name__ == '__main__':
    # ディレクトリの読み込み
    target_dir = sys.argv[1]
    print(target_dir)
    for filename in os.listdir(target_dir):
        path_in = os.path.join(target_dir, filename)
        if os.path.isfile(path_in) and path_in.endswith('.mid') :
            smf2mct(target_dir, filename)
    print('conversion finished.')