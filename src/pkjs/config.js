module.exports = [
    {
        "type": "heading",
        "defaultValue": "Pulsar Watchface Configuraiton",
    }, {
        "type": "text",
        "defaultValue": "Configure the color settings of your watchface.",
    }, {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Colors",
            }, {
                "type": "color",
                "messageKey": "BackgroundColor",
                "defaultValue": "0x000000",
                "label": "Background Color",
            }, {
                "type": "color",
                "messageKey": "ForegroundColor",
                "defaultValue": "0xFF0000",
                "label": "Foreground Color",
            }
        ]
    }, {
        "type": "submit",
        "defaultValue": "Save Settings",
    }
];
