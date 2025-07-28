CREATE TABLE "passengers" (
    "id" INTEGER,
    "first_name" TEXT,
    "last_name" TEXT,
    "age" INTEGER,
    PRIMARY KEY("id")
);

CREATE TABLE "checkin" (
    "date" DATE,
    "flight" INTEGER,
    FOREIGN KEY("flight") REFERENCES "flights"("id")
);

CREATE TABLE "airlines" (
    "id" INTEGER,
    "name" TEXT,
    "concourse" TEXT CHECK("concourse" in ('A', 'B', 'C', 'D', 'E', 'F', 'T')),
    PRIMARY KEY("id")
);

CREATE TABLE "flights" (
    "id" INTEGER,
    "airline" INTEGER,
    "departure_port" TEXT,
    "arrival_port" TEXT,
    "departure_time" TIME,
    "arrival_rime" TIME,
    PRIMARY KEY("id"),
    FOREIGN KEY("airline") REFERENCES "airlines"("id")
);