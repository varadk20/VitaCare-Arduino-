import speech_recognition as sr
import whisper
import pandas as pd
import spacy
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
import numpy as np
import re

#  NLP Model for Text Processing
nlp = spacy.load("en_core_web_sm")

# Load Whisper Model
whisper_model = whisper.load_model("medium")

#  Normalize Symptoms using NLP
def normalize_text(text):
    text = text.lower().strip()
    text = text.replace("_", " ")  # Replace underscores with spaces
    text = re.sub(r'\s+', ' ', text)  # Remove extra spaces
    
    # Process with NLP
    doc = nlp(text)
    normalized_tokens = [token.lemma_ for token in doc]  # Lemmatization
    return " ".join(normalized_tokens)

#  Capture Voice Input and Transcribe
def get_voice_input():
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        print("\n🎙️ Speak your symptoms clearly... (Example: 'fever, cough, breathlessness')")
        recognizer.adjust_for_ambient_noise(source, duration=2)

        print("🎤 Listening... (Speak for up to 20 seconds)")
        audio = recognizer.listen(source, timeout=15, phrase_time_limit=20)

        # Save audio for Whisper processing
        audio_file = "temp_audio.wav"
        with open(audio_file, "wb") as f:
            f.write(audio.get_wav_data())

        # Transcribe with Whisper
        text = transcribe_audio(audio_file)
        print(f"📝 Whisper Transcription: {text}")
        return text

#  Transcribe Audio with Whisper
def transcribe_audio(filename):
    try:
        result = whisper_model.transcribe(filename, language="en")
        return normalize_text(result["text"])
    except Exception as e:
        print(f"⚠️ Whisper Error: {e}")
        return ""


df_symptoms = pd.read_csv(r"C:\Users\ezeki\Downloads\DiseaseAndSymptoms.csv")
df_precautions = pd.read_csv(r"C:\Users\ezeki\Downloads\Disease precaution.csv")

df_symptoms.fillna("", inplace=True)

#  Normalize Symptoms in Dataset
df_symptoms["All Symptoms"] = df_symptoms.iloc[:, 1:].apply(lambda x: ' '.join(x).strip(), axis=1)
df_symptoms_cleaned = df_symptoms[['Disease', 'All Symptoms']].drop_duplicates().reset_index(drop=True)

df_symptoms_cleaned["Disease"] = df_symptoms_cleaned["Disease"].map(normalize_text)
df_symptoms_cleaned["All Symptoms"] = df_symptoms_cleaned["All Symptoms"].map(normalize_text)

#  Similarity Matching
vectorizer = TfidfVectorizer()
symptom_vectors = vectorizer.fit_transform(df_symptoms_cleaned["All Symptoms"])

#Predict Disease 
def predict_disease(user_input):
    user_input_text = normalize_text(user_input)
    user_vector = vectorizer.transform([user_input_text])
    similarities = cosine_similarity(user_vector, symptom_vectors).flatten()

    best_match_index = np.argmax(similarities)
    best_match_score = similarities[best_match_index]

    print(f"🔍 Similarity Score: {best_match_score:.2f}")  # Debugging

    if best_match_score > 0.1:  # Lowered Threshold
        return df_symptoms_cleaned.iloc[best_match_index]["Disease"]
    else:
        return "No exact disease match found"

# Get Precautions
def get_precautions(disease):
    precautions = df_precautions[df_precautions["Disease"].map(normalize_text) == disease]
    return precautions.iloc[0, 1:].dropna().tolist() if not precautions.empty else ["No specific precautions found."]

#  Take User Input ---
print("\n🗣️ Do you want to (1) Speak or (2) Type your symptoms?")
choice = input("Enter 1 for voice input, 2 for text input: ").strip()

if choice == "1":
    user_input = get_voice_input()
    if not user_input:
        print("⚠️ Voice input failed. Switching to text input.")
        user_input = input("Enter symptoms: ")
else:
    user_input = input("Enter symptoms: ")

#  Predict Disease Based on Normalized Symptoms
if not user_input.strip():
    print("\n⚠️ Error: No valid input detected. Please try again.")
else:
    predicted_disease = predict_disease(user_input)
    precautions = get_precautions(predicted_disease)

    print("\n========================================")
    print(f"🔍 Predicted Disease: **{predicted_disease}**")
    print("\n✅ Precautions to be taken:")
    for i, p in enumerate(precautions, 1):
        print(f"{i}. {p}")
    print("========================================")
