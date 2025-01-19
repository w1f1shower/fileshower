import random

def get_random_user_agent():
    """Reads a user agent from a wordlist file and returns a random entry."""
    wordlist_path = "user_agents.txt"
    try:
        with open(wordlist_path, "r") as file:
            user_agents = file.readlines()
        return random.choice(user_agents).strip()
    except FileNotFoundError:
        print("User agent wordlist file not found.")
        return "Mozilla/5.0 (compatible; DefaultUserAgent)"
