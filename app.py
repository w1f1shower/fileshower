from flask import Flask, request, render_template
from app.routes import setup_routes

app = Flask(__name__)

# Register routes
setup_routes(app)

if __name__ == "__main__":
    app.run(debug=True)
