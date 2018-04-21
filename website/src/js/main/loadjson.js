var config = null;

$.getJSON("wrtcr_conf.json", function(data) {
    config = data;
});

function getConfig() {
    return config;
}
