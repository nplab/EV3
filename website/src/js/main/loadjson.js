var config = null;

// Load JSON File
$.getJSON("wrtcr_conf.json", function(data) {
    config = data;
});

function getConfig() {
    return config;
}
