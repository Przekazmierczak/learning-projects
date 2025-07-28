CREATE VIEW "frequently_reviewed" AS
WITH "count_reviews" AS (
    SELECT "listing_id", COUNT(*) AS "sum"
    FROM "reviews"
    GROUP BY "listing_id"
)
SELECT "listings"."id", "listings"."property_type", "listings"."host_name", "count_reviews"."sum" AS "reviews"
FROM "listings"
JOIN "count_reviews" ON "listings"."id" = "count_reviews"."listing_id"
ORDER BY "reviews" DESC, "property_type";