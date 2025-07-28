CREATE VIEW "rural" AS
SELECT "id", "district", "locality", "families", "households", "population", "male", "female"
FROM "census"
WHERE "locality" like '%rural%';