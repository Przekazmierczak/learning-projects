/*
Users
The heart of LinkedIn’s platform is its people. Your database should be able to represent the following information about LinkedIn’s users:

Their first and last name
Their username
Their password
Keep in mind that, if a company is following best practices, application passwords are “hashed.” No need to worry about hashing passwords here, though it might be helpful to know that some hashing algorithms can produce strings up to 128 characters long.

Schools and Universities
LinkedIn also allows for official school or university accounts, such as that for Harvard, so alumni (i.e., those who’ve attended) can identify their affiliation. Ensure that LinkedIn’s database can store the following information about each school:

The name of the school
The type of school
The school’s location
The year in which the school was founded
You should assume that LinkedIn only allows schools to choose one of three types: “Primary,” “Secondary,” and “Higher Education.”

Companies
LinkedIn allows companies to create their own pages, like the one for LinkedIn itself, so employees can identify their past or current employment with the company. Ensure that LinkedIn’s database can store the following information for each company:

The name of the company
The company’s industry
The company’s location
You should assume that LinkedIn only allows companies to choose from one of three industries: “Technology,” “Education,” and “Business.”

Connections
And finally, the essence of LinkedIn is its ability to facilitate connections between people. Ensure LinkedIn’s database can support each of the following connections.

Connections with People
LinkedIn’s database should be able to represent mutual (reciprocal, two-way) connections between users. No need to worry about one-way connections user A “following” user B without user B “following” user A.

Connections with Schools
A user should be able to create an affiliation with a given school. And similarly, that school should be able to find its alumni. Additionally, allow a user to define:

The start date of their affiliation (i.e., when they started to attend the school)
The end date of their affiliation (i.e., when they graduated), if applicable
The type of degree earned/pursued (e.g., “BA”, “MA”, “PhD”, etc.)
Connections with Companies
A user should be able to create an affiliation with a given company. And similarly, a company should be able to find its current and past employees. Additionally, allow a user to define:

The start date of their affiliation (i.e., the date they began work with the company)
The end date of their affiliation (i.e., when left the company), if applicable
*/

CREATE TABLE `users` (
    `id` INT AUTO_INCREMENT,
    `username` VARCHAR(32) NOT NULL UNIQUE,
    `password` VARCHAR(128) NOT NULL,
    PRIMARY KEY(`id`)
);

CREATE TABLE `schools` (
    `id` INT AUTO_INCREMENT,
    `name` VARCHAR(64) NOT NULL UNIQUE,
    `type` ENUM('Primary', 'Secondary', 'Higher Education') NOT NULL,
    `location` VARCHAR(64) NOT NULL,
    `year` YEAR NOT NULL,
    PRIMARY KEY(`id`)
);

CREATE TABLE `companies` (
    `id` INT AUTO_INCREMENT,
    `name` VARCHAR(64) NOT NULL UNIQUE,
    `type` ENUM('Technology', 'Education', 'Business') NOT NULL,
    `location` VARCHAR(64) NOT NULL,
    PRIMARY KEY(`id`)
);

CREATE TABLE `people_connection` (
    `user` INT,
    `friend` INT,
    FOREIGN KEY(`user`) REFERENCES `users`(`id`),
    FOREIGN KEY(`friend`) REFERENCES `users`(`id`)
);

CREATE TABLE `schools_connection` (
    `user` INT,
    `school` INT,
    `start` DATE NOT NULL,
    `end` DATE NOT NULL,
    `type` VARCHAR(32) NOT NULL,
    FOREIGN KEY(`user`) REFERENCES `users`(`id`),
    FOREIGN KEY(`school`) REFERENCES `schools`(`id`)
);

CREATE TABLE `companies_connection` (
    `user` INT,
    `company` INT,
    `start` DATE NOT NULL,
    `end` DATE NOT NULL,
    FOREIGN KEY(`user`) REFERENCES `users`(`id`),
    FOREIGN KEY(`company`) REFERENCES `companies`(`id`)
);