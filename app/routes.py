from flask import Blueprint, request, jsonify, render_template
from app.utils.request_modifier import deliver_payload
from app.utils.metadata_injector import add_metadata

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


@app.route("/inject_metadata", methods=["POST"])
def inject_metadata():
    uploaded_file = request.files.get("file")
    metadata = request.form.get("metadata", "")

    if not uploaded_file or not metadata:
        return jsonify({"error": "File and metadata are required!"}), 400

    # Save the uploaded file
    file_path = os.path.join(app.config["UPLOAD_FOLDER"], uploaded_file.filename)
    uploaded_file.save(file_path)

    # Call the add_metadata function to inject metadata
    try:
        add_metadata(file_path, metadata)
    except Exception as e:
        return jsonify({"error": f"Failed to inject metadata: {str(e)}"}), 500

    return jsonify({"message": "Metadata injected successfully!", "file_path": file_path})
