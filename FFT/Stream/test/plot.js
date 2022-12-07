$(()=>{
    const ctxLog = $('#chartLog');
    const ctx = $('#chart');

    var chart_config = {
        type: 'scatter',
        data: {
            datasets: [{
                label: "FFT",
                data: [],
                showLine: true,
                tension: 0
            }]
        },
        options: {
            animation: {
                duration: 200,
            }, // general animation time
            hover: {
                animationDuration: 0,
            }, // duration of animations when hovering an item
            responsiveAnimationDuration: 0, // animation duration after a resize
            tooltips: {
                mode: 'index',
                intersect: false,
                footerFontStyle: 'normal'
            },
            responsive: true,
            scales: {
                x: {
                    min: 0,
                    max: 0
                },
                y: {
                    beginAtZero: true
                }
            },
            elements: {
                point: {
                    radius: 0
                }
            }
        },
        plugins: []
    };

    var chartLog_config = JSON.parse(JSON.stringify(chart_config));
    chartLog_config.options.scales.y = {
        // type: 'logarithmic'
    };
    var chartLog = new Chart(ctxLog, chartLog_config);
    var chart = new Chart(ctx, chart_config);

    function mapToChartCoords(chart, x, y){
        var ytop = chart.chartArea.top;
        var ybottom = chart.chartArea.bottom;
        var ymin = chart.scales.y.min;
        var ymax = chart.scales.y.max;
        var newy = '';
        var showstuff = 0;
        if (y <= ybottom && y >= ytop) {
            newy = Math.abs((y - ytop) / (ybottom - ytop));
            newy = (newy - 1) * -1;
            newy = newy * (Math.abs(ymax - ymin)) + ymin;
            showstuff = 1;
        }
        var xtop = chart.chartArea.left;
        var xbottom = chart.chartArea.right;
        var xmin = chart.scales.x.min;
        var xmax = chart.scales.x.max;
        var newx = '';
        if (x <= xbottom && x >= xtop && showstuff == 1) {
            newx = Math.abs((x - xtop) / (xbottom - xtop));
            newx = newx * (Math.abs(xmax - xmin)) + xmin;
        }
        return {newx, newy};
    }

    ctxLog.mousemove(function(evt) {
        // console.log(evt.offsetX + "," + evt.offsetY);
        var obj = mapToChartCoords(chartLog, evt.offsetX, evt.offsetY);

        if (obj.newy != '' && obj.newx != '') {
            //console.log(newx + ',' + newy);
            $("#chartLog_graph_coords").html('Coordinates: ' + obj.newx.toFixed(2) + ' Hz, ' + obj.newy.toFixed(2)+" Signal");
        }
    });

    ctx.mousemove(function(evt) {
        var obj = mapToChartCoords(chart, evt.offsetX, evt.offsetY);

        if (obj.newy != '' && obj.newx != '') {
            //console.log(newx + ',' + newy);
            $("#chart_graph_coords").html('Coordinates: ' + obj.newx.toFixed(2) + ' Hz, ' + obj.newy.toFixed(2)+" Signal");
        }
    });

    function parseFrequencies(data){
        var frequencies = [];
        data.freq.forEach(element => {
            frequencies.push(math.number(element));
        });
        return frequencies;
    }

    function parseMagnitudeLog(data) {
        var magnitudes = [];
        data.fft.forEach(element => {
            var cmplx = math.complex(element);
            var abs = math.sqrt(math.pow(cmplx.re, 2) + math.pow(cmplx.im, 2));
            var value = -100;
            if(abs > 0){
                value = 20*math.log10(abs);
            }
            magnitudes.push(value);
        });
        return magnitudes;
    }

    function parseMagnitude(data) {
        var magnitudes = [];
        data.fft.forEach(element => {
            var cmplx = math.complex(element);
            var value = math.sqrt(math.pow(cmplx.re, 2) + math.pow(cmplx.im, 2));
            magnitudes.push(value);
        });
        return magnitudes;
    }

    function updateFFTChart(chart, magnitudes, frequencies){
        var _chart_data = [];
        var display_size_ = magnitudes.length;
        for(var i = 0; i < display_size_; i++) {
            _chart_data.push({x: Number(frequencies[i]), y: Number(magnitudes[i])});
        }
        chart.options.scales.x.max = frequencies[frequencies.length - 1];
        chart.data.datasets[0].data = _chart_data;
        $("#max_freq").html(frequencies[frequencies.length-1].toFixed(2));
        chart.update();
    }

    function plotDataLog(data){
        magnitudes = parseMagnitudeLog(data);
        frequencies = parseFrequencies(data);
        updateFFTChart(chartLog, magnitudes, frequencies);
    }

    function plotData(data){
        magnitudes = parseMagnitude(data);
        frequencies = parseFrequencies(data);
        updateFFTChart(chart, magnitudes, frequencies);
    }

    function get_data(){
        $.ajax({
            method: 'GET',
            url: "http://127.0.0.1:8801/data",
            dataType: 'json',
            contentType: 'text/plain; charset=utf-8',
            success: (data, textStatus, jqXHR)=>{
                plotData(data);
                plotDataLog(data);
                $("#sinFreq").html(data.sinFreq.toFixed(2));
                $("#dominantFreq").html(data.dominantFreq.toFixed(2));
            }
        });
    }

    get_data();

    setInterval(()=>{
        get_data();
    },1000);

});
