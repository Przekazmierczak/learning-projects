/*
Any empty values in meteorites.csv are represented by NULL in the meteorites table.
Keep in mind that the mass, year, lat, and long columns have empty values in the CSV.

All columns with decimal values (e.g., 70.4777) should be rounded to the nearest hundredths place (e.g., 70.4777 becomes 70.48).
Keep in mind that the mass, lat, and long columns have decimal values.

All meteorites with the nametype “Relict” are not included in the meteorites table.
The meteorites are sorted by year, oldest to newest, and then—if any two meteorites landed in the same year—by name, in alphabetical order.

You’ve updated the IDs of the meteorites from meteorites.csv, according to the order specified in #4.
The id of the meteorites should start at 1, beginning with the meteorite that landed in the oldest year and is the first in alphabetical order for that year.
*/

.import --csv meteorites.csv mete_temp

UPDATE "mete_temp"
SET "mass" = NULL
WHERE "mass" = "";

UPDATE "mete_temp"
SET "year" = NULL
WHERE "year" = "";

UPDATE "mete_temp"
SET "lat" = NULL
WHERE "lat" = "";

UPDATE "mete_temp"
SET "long" = NULL
WHERE "long" = "";

UPDATE "mete_temp"
SET "mass" = ROUND("mass", 2);

UPDATE "mete_temp"
SET "lat" = ROUND("lat", 2);

UPDATE "mete_temp"
SET "long" = ROUND("long", 2);

DELETE FROM "mete_temp"
WHERE "nametype" = 'Relict';

CREATE TABLE "meteorites" (
    "id" INTEGER,
    "name",
    "class",
    "mass",
    "discovery",
    "year",
    "lat",
    "long",
    PRIMARY KEY("id")
);

INSERT INTO "meteorites" (
    "name",
    "class",
    "mass",
    "discovery",
    "year",
    "lat",
    "long")
SELECT
    "name",
    "class",
    "mass",
    "discovery",
    "year",
    "lat",
    "long"
FROM "mete_temp"
ORDER BY "year", "name";

SELECT * FROM "meteorites" LIMIT 1000;