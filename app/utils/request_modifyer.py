import requests
import json
from app.utils.user_agent_rotator import get_random_user_agent

def deliver_payload(url, file_path, headers, use_random_user_agent=False):
    with open(file_path, "rb") as f:
        files = {"file": (file_path.split("/")[-1], f)}

        # Parse headers
        if headers:
            headers = json.loads(headers)
        else:
            headers = {}

        # Add random user agent
        if use_random_user_agent:
            headers["User-Agent"] = get_random_user_agent()

        # Send request
        response = requests.post(url, files=files, headers=headers)

    return {"status_code": response.status_code, "response": response.text}
