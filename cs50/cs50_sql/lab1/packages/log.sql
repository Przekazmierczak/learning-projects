
-- *** The Lost Letter ***
SELECT "to_address_id", "contents"
FROM "packages"
WHERE "contents" LIKE '%congratulatory%';

SELECT "address", "type"
FROM "addresses"
WHERE "id" = (
    SELECT "to_address_id"
    FROM "packages"
    WHERE "contents" LIKE '%congratulatory%'
);
-- *** The Devious Delivery ***
SELECT "contents"
FROM "packages"
WHERE "from_address_id"  NULL;

SELECT "address_id"
FROM "scans"
WHERE "package_id" = (
    SELECT "id"
    FROM "packages"
    WHERE "from_address_id" IS NULL
);

SELECT "type"
FROM "addresses"
WHERE "id" = 348;

-- *** The Forgotten Gift ***
SELECT "id", "address"
FROM "addresses"
WHERE "address" LIKE '109 Tileston Street';

SELECT "id", "contents"
FROM "packages"
WHERE "from_address_id" = (
    SELECT "id"
    FROM "addresses"
    WHERE "address" LIKE '109 Tileston Street'
);

SELECT "driver_id", "action", "timestamp"
FROM "scans"
WHERE "package_id" = (
    SELECT "id"
    FROM "packages"
    WHERE "from_address_id" = (
        SELECT "id"
        FROM "addresses"
        WHERE "address" LIKE '109 Tileston Street'
    )
);

SELECT "name"
FROM "drivers"
WHERE "id" = 17;