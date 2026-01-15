PRAGMA foreign_keys = OFF;

DROP TABLE IF EXISTS Messages;
DROP TABLE IF EXISTS Channels;
DROP TABLE IF EXISTS Categories;
DROP TABLE IF EXISTS Users;
DROP TABLE IF EXISTS Roles;

PRAGMA foreign_keys = ON;

CREATE TABLE Roles (
    RoleID INTEGER PRIMARY KEY AUTOINCREMENT,
    RoleName TEXT NOT NULL UNIQUE
);

INSERT INTO Roles (RoleName)
VALUES
    ('User'),
    ('Moderator'),
    ('Administrator');

CREATE TABLE Users (
    UserID INTEGER PRIMARY KEY AUTOINCREMENT,
    IsConnected INTEGER NOT NULL DEFAULT 0,
    UserName TEXT NOT NULL UNIQUE,
    UserRole INTEGER DEFAULT 1,

    CHECK(IsConnected IN (0,1)),
    FOREIGN KEY (UserRole) REFERENCES Roles(RoleID)
);

INSERT INTO Users (UserName)
VALUES
    ('ROOT');

CREATE TABLE Categories (
    CategoryID INTEGER PRIMARY KEY,
    CategoryName TEXT NOT NULL UNIQUE,
    Position INTEGER NOT NULL UNIQUE
);

CREATE TABLE Channels (
    ChannelID INTEGER PRIMARY KEY,
    Position INTEGER NOT NULL,
    SalonName TEXT NOT NULL,
    CategoryID INTEGER NOT NULL,

    FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID)
);

CREATE TABLE Messages (
    MessageID INTEGER PRIMARY KEY,
    Content TEXT,
    ChannelID INTEGER NOT NULL,
    CreatedAt TEXT DEFAULT (CURRENT_TIMESTAMP),
    IsVisible INTEGER NOT NULL,
    AuthorID INTEGER NOT NULL,
    EditedDate TEXT DEFAULT NULL,

    CHECK(IsVisible IN (0, 1)),
    FOREIGN KEY (ChannelID) REFERENCES Channels(ChannelID),
    FOREIGN KEY (AuthorID) REFERENCES Users(UserID)
);
