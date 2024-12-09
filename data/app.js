function setup() {
  function init() {
    this.data = new Array(this.width * this.height).fill([0, 0, 0]);
  }
  function changeColor(event) {
    this.color = hexToRgb(event.target.value);
  }

  function hexToRgb(color) {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(color);
    return result ? [
      parseInt(result[1], 16),
      parseInt(result[2], 16),
      parseInt(result[3], 16)
    ] : null;
  }

  function rgbToHex(color) {
    return "#" + (1 << 24 | color[0] << 16 | color[1] << 8 | color[2]).toString(16).slice(1);
  }

  function setColor(index, forse = false) {
    if (!this.isMouseDown && !forse) {
      return;
    }
    this.data[index] = this.color
  }

  function send() {
    fetch('http://192.168.1.152/data', {
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
      },
      method: "POST",
      body: JSON.stringify(this.data)
    })
  }

  return {
    width: 16,
    height: 16,
    color: [0, 0, 0],
    data: [],
    isMouseDown: false,

    init,
    rgbToHex,
    hexToRgb,
    changeColor,
    setColor,
    send
  }
}