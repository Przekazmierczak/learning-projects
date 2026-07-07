// Variables used by Scriptable.
// These must be at the very top of the file. Do not edit.
// icon-color: red; icon-glyph: magic;

// Globals
const REFRESHHOUR = 6

const ICON = "pills.fill"
const ICON_SIZE = 40

const NAME_1 = "Medicine"
const NAME_2 = "Tracker"

const FONT_SIZE = 14

// Load file
const fm = FileManager.local()

const folder = fm.documentsDirectory()
const file = fm.joinPath(folder, "data.json")

const fileText  = fm.readString(file)
const items  = JSON.parse(fileText)

// SET WIDGET
const widget = new ListWidget()

widget.backgroundColor = new Color("#000000")

addTitle()

for (const item of items) {
    addItem(item)
}

setRefresh()

// Show widget
Script.setWidget(widget)

Script.complete()

function addTitle() {
    const row = widget.addStack()
    row.layoutHorizontally()
    row.centerAlignContent()

    // ICON
    const pillSymbol = SFSymbol.named(ICON)
    const pillImage = row.addImage(pillSymbol.image)

    pillImage.imageSize = new Size(ICON_SIZE , ICON_SIZE )
    // pillImage.tintColor = Color.white()

    row.addSpacer(8)

    // Text stack
    const titleStack = row.addStack()
    titleStack.layoutVertically()

    const title1 = titleStack.addText(NAME_1)
    title1.textColor = Color.blue()
    title1.font = Font.boldSystemFont(FONT_SIZE)

    const title2 = titleStack.addText(NAME_2)
    title2.textColor = Color.blue()
    title2.font = Font.boldSystemFont(FONT_SIZE)
}

function addItem(item) {
    const now = new Date()
    const date = new Date(item.updated)

    const diffDays = Math.floor((now - date) / 1000 / 60 / 60 / 24)
    const used = Math.floor(diffDays / item.frequency)
    const left = item.quantity - used

    const text = widget.addText(
    `${item.name}: ${left}`
    )

    const leftDays = left * item.frequency

    if (leftDays >= 30) {
        text.textColor = Color.white()
    } else if (leftDays > 14) {
        text.textColor = Color.yellow()
    } else {
        text.textColor = Color.red()
    }

    text.font = Font.systemFont(14)
    // text.centerAlignText()
    widget.addSpacer(4)
}

function setRefresh() {
    const nextRefresh = new Date()

    nextRefresh.setHours(REFRESHHOUR, 0, 0, 0)

    if (nextRefresh <= new Date()) {
        nextRefresh.setDate(
            nextRefresh.getDate() + 1
        )
    }

    widget.refreshAfterDate = nextRefresh
}