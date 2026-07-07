// Variables used by Scriptable.
// These must be at the very top of the file. Do not edit.
// icon-color: deep-blue; icon-glyph: magic;
const fm = FileManager.local()

const folder = fm.documentsDirectory()
const file = fm.joinPath(folder, "data.json")

const items = [
  {
    name: "item1",
    quantity: 31,
    frequency: 1,
    updated: "2026-07-04"
  },
  {
    name: "item2",
    quantity: 15,
    frequency: 1,
    updated: "2026-07-04"
  },
  {
    name: "item3",
    quantity: 10,
    frequency: 1,
    updated: "2026-07-04"
  },
  {
    name: "item4",
    quantity: 15,
    frequency: 7,
    updated: "2026-07-04"
  }
]

// SAVE
fm.writeString(
  file,
  JSON.stringify(items, null, 2)
)