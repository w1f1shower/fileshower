from flask import Blueprint, request, jsonify, render_template
from app.utils.request_modifier import deliver_payload
from app.utils.payload_creator import create_payload_file

app = Blueprint("app", __name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/deliver", methods=["POST"])
def deliver():
    url = request.form.get("url")
    file_path = request.form.get("file_path")
    headers = request.form.get("headers", "{}")
    use_random_user_agent = request.form.get("use_random_user_agent") == "on"
    result = deliver_payload(url, file_path, headers, use_random_user_agent)
    return jsonify(result)

@app.route("/create_payload", methods=["POST"])
def create_payload():
    file_type = request.form.get("file_type")
    metadata = request.form.get("metadata", "")
    file_path = create_payload_file(file_type, metadata)
    return jsonify({"file_path": file_path})
