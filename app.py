from flask import Flask, request, redirect
import csv         
import subprocess   
import os           

app = Flask(__name__)

# Ensure the folder for images exists
os.makedirs("static", exist_ok=True)

# Create data.csv with a header if it doesn't exist yet
if not os.path.exists("data.csv"):
    with open("data.csv", "w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        # These headers are skipped by the C++ dummy getline
        writer.writerow(["Status", "Date", "Category", "Color", "Location", "Description", "Image_name"])

@app.route('/')
def home():
    with open("index.html", "r", encoding="utf-8") as file:
        html_content = file.read()
    
    match_alerts = "<p style='color: #aaaaaa;'>System standing by... waiting for matches.</p>"
    
    if os.path.exists("matches.txt"):
        with open("matches.txt", "r", encoding="utf-8") as file:
            content = file.read().strip()
            if content:  # If the file isn't empty, use its contents
                match_alerts = content
            
    # Inject the matches into the HTML
    html_content = html_content.replace("<!-- MATCHES_GO_HERE -->", match_alerts)
    
    return html_content

@app.route('/submit', methods=['POST'])
def submit():
    status = request.form['status']
    date = request.form['date']
    category = request.form['category']
    color = request.form['color']
    location = request.form['location']
    
    # CRITICAL FIX: Strip commas from description so C++ doesn't break
    desc = request.form['desc'].replace(",", " ")
    
    image = request.files['image'] 
    image_filename = image.filename.replace(" ", "_") 
    
    if image_filename:
        image.save(os.path.join("static", image_filename))
    
    # Save to CSV
    with open("data.csv", "a", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow([status, date, category, color, location, desc, image_filename])
        
    # Run the C++ engine (Assuming Windows. If Mac/Linux, change to "./engine")
    subprocess.run(["engine.exe"], shell=True) 
    
    return redirect('/')

if __name__ == '__main__':
    app.run(debug=True)