SELECT "city", COUNT("city") AS "number of public schools"
FROM "schools"
WHERE "type" = "Public School"
GROUP BY "city"
HAVING "number of public schools" <= 3
ORDER BY "number of public schools" DESC, "city";