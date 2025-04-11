import pandas as pd
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
import numpy as np
import re
from fuzzywuzzy import process  # Install with: pip install fuzzywuzzy

# Function to normalize symptoms (remove underscores, fix spacing, lowercase)
def normalize_text(text):
    """ Converts text to lowercase, replaces underscores and removes extra spaces. """
    text = str(text).lower().strip()
    text = re.sub(r'[_\s]+', ' ', text)  # Replace underscores and multiple spaces with a single space
    text = text.replace(" ", "")  # Ensure symptoms like 'high fever' and 'highfever' match
    return text

# Load datasets
df_symptoms = pd.read_csv(r"C:\Users\ezeki\Downloads\DiseaseAndSymptoms.csv")  # Update path
df_precautions = pd.read_csv(r"C:\Users\ezeki\Downloads\Disease precaution.csv")  # Update path

# Preprocessing
df_symptoms.fillna("", inplace=True)

# Normalize symptoms in the dataset
for col in df_symptoms.columns[1:]:  # Skip 'Disease' column
    df_symptoms[col] = df_symptoms[col].map(normalize_text)

# Combine all symptoms into a single text field
df_symptoms["All Symptoms"] = df_symptoms.iloc[:, 1:].apply(lambda x: ' '.join(x).strip(), axis=1)
df_symptoms_cleaned = df_symptoms[['Disease', 'All Symptoms']].drop_duplicates().reset_index(drop=True)

# Normalize disease names
df_symptoms_cleaned["Disease"] = df_symptoms_cleaned["Disease"].map(normalize_text)

# TF-IDF Vectorization
vectorizer = TfidfVectorizer()
symptom_vectors = vectorizer.fit_transform(df_symptoms_cleaned["All Symptoms"])

# Get all unique symptoms from the dataset
all_symptoms = set()
for symptoms in df_symptoms_cleaned["All Symptoms"]:
    all_symptoms.update(symptoms.split())

# Function to correct spelling mistakes using fuzzy matching
def correct_symptoms(user_symptoms):
    """ Matches user input symptoms to closest correct symptoms in the dataset. """
    corrected_symptoms = []
    for symptom in user_symptoms:
        match, score = process.extractOne(symptom, all_symptoms)  # Find best match
        if score > 80:  # If confidence is high, replace with correct symptom
            corrected_symptoms.append(match)
        else:
            corrected_symptoms.append(symptom)  # Keep original if no good match found
    return corrected_symptoms

def predict_disease(user_symptoms):
    """ Predicts the most likely disease based on user symptoms using cosine similarity. """
    user_input_text = ' '.join(correct_symptoms([normalize_text(s) for s in user_symptoms])).strip()
    user_vector = vectorizer.transform([user_input_text])
    
    similarities = cosine_similarity(user_vector, symptom_vectors).flatten()
    
    # Get the best matching disease
    best_match_index = np.argmax(similarities)
    predicted_disease = df_symptoms_cleaned.iloc[best_match_index]["Disease"]
    
    return predicted_disease

def get_precautions(disease):
    """ Fetches precautions for a given disease. """
    precautions = df_precautions[df_precautions["Disease"].map(normalize_text) == disease]
    if precautions.empty:
        return ["No specific precautions found."]
    return precautions.iloc[0, 1:].dropna().tolist()

# 🔹 Take input from user
user_input = input("Enter symptoms separated by commas: ").split(',')

# 🔹 Normalize and correct user symptoms
user_input = correct_symptoms([normalize_text(symptom) for symptom in user_input])

# 🔹 Predict disease
predicted_disease = predict_disease(user_input)

# 🔹 Get precautions
precautions = get_precautions(predicted_disease)

# 🔹 Show results
print("\n========================================")
print(f"🔍 Predicted Disease: **{predicted_disease}**")

print("\n✅ Precautions to be taken:")
for i, p in enumerate(precautions, 1):
    print(f"{i}. {p}")

print("========================================")
