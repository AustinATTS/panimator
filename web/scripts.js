// Source coordinate system size.
// All circle positions/radii are defined relative to this grid.
const SRC_WIDTH = 97;
const SRC_HEIGHT = 36;

// Canvas + 2D drawing context.
const canvas = document.getElementById("circleCanvas");
const context = canvas.getContext("2d");

// Holds the parsed contents of circles.json.
let circlesData = [];

// Calculate a uniform scale factor so the source grid
// fits inside the canvas without distortion.
function getScale() {
    return Math.min(canvas.width / SRC_WIDTH, canvas.height / SRC_HEIGHT);
}

// Main render loop (runs every animation frame).
function render() {
    // Clear the entire canvas
    context.clearRect(0, 0, canvas.width, canvas.height);

    const scale = getScale();

    // Group circles by colour so we can batch-draw them.
    // This reduces state changes and improves performance.
    const buckets = new Map();

    for (const c of circlesData) {
        // Ignore unsupported shapes
        if (c.shape !== "circle") {
            continue;
        }

        // Use provided colour, or fall back to orange.
        const colour = c.parameters.colour ?? "orange";

        // Create a shared path for this colour if needed.
        if (!buckets.has(colour)) {
            buckets.set(colour, new Path2D());
        }

        const path = buckets.get(colour);

        // Convert source coordinates into canvas space.
        const x = c.parameters.x * scale;
        const y = c.parameters.y * scale;

        // Default radius if none is provided.
        const radius = (c.parameters.radius || 0.45) * scale;

        // Draw the circle into the Path2D.
        path.moveTo(x + radius, y);
        path.arc(x, y, radius, 0, Math.PI * 2);
    }

    // Fill each colour bucket in one go.
    for (const [colour, path] of buckets) {
        context.fillStyle = colour;
        context.fill(path);
    }

    // Schedule the next frame.
    requestAnimationFrame(render);
}

// Periodically fetch updated circle data from disk.
function updateJSON() {
    // Cache-busting query param ensures we always get fresh data.
    fetch("circles.json?ts=" + Date.now())
        .then(response => response.json())
        .then(data => {
            circlesData = data;
        })
        .catch(error => console.warn("Failed to fetch circles.json", error));
}

// Refresh JSON ~30 times per second.
setInterval(updateJSON, 33);

// Start rendering immediately.
render();
