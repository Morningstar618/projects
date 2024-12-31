from flask_sqlalchemy import SQLAlchemy
from itsdangerous.url_safe import URLSafeTimedSerializer as Serializer
from flask_login import UserMixin
from datetime import datetime
import os

# This is the database object that is used to interact with the database.
db = SQLAlchemy()

class Users(db.Model, UserMixin):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(20), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    image_file = db.Column(db.String(20), nullable=False, default="default.jpg")
    password = db.Column(db.String(60), nullable=False)
    # This is a relationship between the User and Post models. This is a one-to-many relationship.
    # This relationship is not a column in the database, but a query that runs in the background.
    # It is used to get all the posts from a specific user.
    posts = db.relationship("Posts", backref="author", lazy=True)

    def get_reset_token(self):
        secret = os.environ.get("SECRET_KEY")
        s = Serializer(secret)
        return s.dumps({"user_id": self.id})
    
    @staticmethod
    def verify_reset_token(token):
        secret = os.environ.get("SECRET_KEY")
        s = Serializer(secret)
        try:
            user_id = s.loads(token)["user_id"]
        except:
            return None
        return Users.query.get(user_id)

    def __repr__(self):
        return f"User('{self.username}', '{self.email}', '{self.image_file}')"
    

class Posts(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    title = db.Column(db.String(100), nullable=False)
    date_posted = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    content = db.Column(db.Text, nullable=False)
    # This is a foreign key that references the id column in the user table.
    # This is used to get the user who created the post and to get all the posts from a specific user.
    user_id = db.Column(db.Integer, db.ForeignKey("users.id"), nullable=False)

    def __repr__(self):
        return f"Post('{self.title}', '{self.date_posted}')"
    

# INSERT INTO users (username, email, image_file, password) VALUES ('dummy', 'dummy@demo.com', 'default.jpg', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8');
