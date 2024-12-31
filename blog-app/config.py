import os
from dotenv import load_dotenv

load_dotenv()

class Config:
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL', 'postgresql://username:password@hostname:port/database_name')
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    # This is a secret key that is used to protect against modifying cookies and cross-site request forgery attacks.
    # Used in registration and login forms
    SECRET_KEY = os.environ.get('SECRET_KEY', 'your_secret_key')