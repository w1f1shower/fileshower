from flask import Flask
import os


def create_app():
    app = Flask(__name__)

    app.config["UPLOAD_FOLDER"] = os.path.join(os.getcwd(), "app/uploads")
    os.makedirs(app.config["UPLOAD_FOLDER"], exist_ok=True)

    from app.routes import app as routes_blueprint
    app.register_blueprint(routes_blueprint)


    return app
