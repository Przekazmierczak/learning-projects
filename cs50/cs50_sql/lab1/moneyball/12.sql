SELECT * FROM (
    SELECT "players"."first_name", "players"."last_name"
    FROM "players"
    JOIN "salaries" ON "salaries"."player_id" = "players"."id"
    JOIN "performances" ON "salaries"."player_id" = "performances"."player_id"
    AND "salaries"."year" = "performances"."year"
    WHERE "performances"."year" = 2001 AND "performances"."H" != 0
    ORDER BY "salaries"."salary" / "performances"."H", "players"."first_name", "players"."last_name"
    LIMIT 10
)
INTERSECT
SELECT * FROM (
    SELECT "players"."first_name", "players"."last_name"
    FROM "players"
    JOIN "salaries" ON "salaries"."player_id" = "players"."id"
    JOIN "performances" ON "salaries"."player_id" = "performances"."player_id"
    AND "salaries"."year" = "performances"."year"
    WHERE "performances"."year" = 2001 AND "performances"."RBI" != 0
    ORDER BY "salaries"."salary" / "performances"."RBI", "players"."first_name", "players"."last_name"
    LIMIT 10
)
ORDER BY "last_name";