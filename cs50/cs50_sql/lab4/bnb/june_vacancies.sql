CREATE VIEW "june_vacancies" AS
WITH "june" AS (
    SELECT "listing_id", COUNT(*) AS "sum"
    FROM "availabilities"
    WHERE "date" LIKE '2023-06-__' AND "available" = 'TRUE'
    GROUP BY "listing_id"
)
SELECT "listings"."id", "listings"."property_type", "listings"."host_name", "june"."sum" AS "days_vacant"
FROM "listings"
JOIN "june" ON "listings"."id" = "june"."listing_id";