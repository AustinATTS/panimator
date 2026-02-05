// TODO(Austin): Add comments.
const SRC_WIDTH = 97;
const SRC_HEIGHT = 36;

const canvas = document.getElementById("circleCanvas");
const context = canvas.getContext("2d");

let circlesData = [];

function getScale() {
    return Math.min(canvas.width / SRC_WIDTH, canvas.height / SRC_HEIGHT);
}

function render() {
    context.clearRect(0, 0, canvas.width, canvas.height);

    const scale = getScale();
    const buckets = new Map();

    for (const c of circlesData) {
        if (c.shape !== "circle") {
            continue;
        }

        const colour = c.parameters.colour ?? "orange";

        if (!buckets.has(colour)) {
            buckets.set(colour, new Path2D());
        }

        const path = buckets.get(colour);

        const x = c.parameters.x * scale;
        const y = c.parameters.y * scale;
        const radius = (c.parameters.radius || 0.45) * scale;

        path.moveTo(x + radius, y);
        path.arc(x, y, radius, 0, Math.PI * 2);
    }

    for (const [colour, path] of buckets) {
        context.fillStyle = colour;
        context.fill(path);
    }

    requestAnimationFrame(render);
}


function updateJSON() {
    fetch("circles.json?ts=" + Date.now())
        .then(response => response.json())
        .then(data => {
            circlesData = data;
        })
        .catch(error => console.warn("Failed to fetch circles.json", error));
}

setInterval(updateJSON, 33);
render();
