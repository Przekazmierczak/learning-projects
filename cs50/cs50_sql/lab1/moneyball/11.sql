SELECT "players"."first_name", "players"."last_name",
"salaries"."salary" / "performances"."H" AS "dollars per hit"
FROM "players"
JOIN "salaries" ON "salaries"."player_id" = "players"."id"
JOIN "performances" ON "salaries"."player_id" = "performances"."player_id"
AND "salaries"."year" = "performances"."year"
WHERE "performances"."year" = 2001 AND "performances"."H" != 0
ORDER BY "dollars per hit", "players"."first_name", "players"."last_name"
LIMIT 10;