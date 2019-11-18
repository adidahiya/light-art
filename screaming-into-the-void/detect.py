#!/usr/bin/env python3

import speech_recognition as sr

# output for TouchDesigner text DAT
prev = sys.stdout
sys.stdout = op('text1')

moral_emotional_words = [
    "abuse",
    "afraid"
    "agony",
    "alarm",
    "alike",
    "ally",
    "along",
    "annoy",
    "apply",
    "argue",
    "asset",
    "awful",
    "best",
    "bias",
    "bunch",
    "calm",
    "cheat",
    "church",
    "clean",
    "coast",
    "crazy",
    "crime",
    "crude",
    "cruel",
    "date",
    "demon",
    "devil",
    "dirt",
    "drum",
    "dull",
    "dumb",
    "empty",
    "enemy",
    "enjoy",
    "ethic",
    "even",
    "evil",
    "fail",
    "fair",
    "faith",
    "fare",
    "fear",
    "filth",
    "foam",
    "focus",
    "form",
    "goat",
    "god",
    "good",
    "great",
    "grief",
    "grim",
    "harm",
    "hate",
    "heal",
    "hell",
    "help",
    "hero",
    "hike",
    "hint",
    "holy",
    "honor",
    "hurt",
    "icon",
    "jail",
    "joke",
    "judge",
    "jury",
    "kill",
    "killed",
    "law",
    "lewd",
    "liar",
    "loss",
    "love",
    "loyal",
    "lust",
    "maze",
    "mercy",
    "mess",
    "miss",
    "moral",
    "nasty",
    "novel",
    "olive",
    "pain",
    "pile",
    "press",
    "pure",
    "rank",
    "rebel",
    "right",
    "ruin",
    "sad",
    "safe",
    "saint",
    "save",
    "scale",
    "score",
    "shame",
    "shape",
    "sin",
    "sleep",
    "solid",
    "spite",
    "spy",
    "steal",
    "suite",
    "swell",
    "taint",
    "theft",
    "tile",
    "title",
    "walk",
    "warm",
    "wave",
    "weak",
    "weep",
    "whore",
    "wrong"
]

# obtain audio from the microphone
r = sr.Recognizer()
with sr.Microphone() as source:
    print("Say something!")
    r.adjust_for_ambient_noise(source)
    audio = r.listen(source)

# recognize speech using Sphinx
try:
    words = r.recognize_sphinx(audio)
    print("Sphinx thinks you said " + words)

    has_moral_emotional_word = False
    for w in words:
        if w in moral_emotional_words:
            print("RESULT_YES")
            has_moral_emotional_word = True

    if not has_moral_emotional_word:
        print("RESULT_NO")

except sr.UnknownValueError:
    print("Sphinx could not understand audio")

except sr.RequestError as e:
    print("Sphinx error; {0}".format(e))

sys.stdout = prev

