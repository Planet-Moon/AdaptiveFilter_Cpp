$(()=>{
    const ctxLogFFT = $('#chartLogFFT');
    const ctxFFT = $('#chartFFT');
    const ctxSignal = $('#chartSignal');

    var chart_fft_config = {
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

    var chartFFTLog_config = JSON.parse(JSON.stringify(chart_fft_config));
    chartFFTLog_config.options.scales.y = {
        // type: 'logarithmic'
    };
    var chartLogFFT = new Chart(ctxLogFFT, chartFFTLog_config);
    var chartFFT = new Chart(ctxFFT, chart_fft_config);

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

    ctxLogFFT.mousemove(function(evt) {
        // console.log(evt.offsetX + "," + evt.offsetY);
        var obj = mapToChartCoords(chartLogFFT, evt.offsetX, evt.offsetY);

        if (obj.newy != '' && obj.newx != '') {
            //console.log(newx + ',' + newy);
            $("#chartLog_graph_coords").html('Coordinates: ' + obj.newx.toFixed(2) + ' Hz, ' + obj.newy.toFixed(2)+" Signal");
        }
    });

    ctxFFT.mousemove(function(evt) {
        var obj = mapToChartCoords(chartFFT, evt.offsetX, evt.offsetY);

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

    function plotDataFFTLog(data){
        magnitudes = parseMagnitudeLog(data);
        frequencies = parseFrequencies(data);
        updateFFTChart(chartLogFFT, magnitudes, frequencies);
    }

    function plotDataFFT(data){
        magnitudes = parseMagnitude(data);
        frequencies = parseFrequencies(data);
        updateFFTChart(chartFFT, magnitudes, frequencies);
    }

    var chart_signal_config = {
        type: 'scatter',
        data: {
            datasets: [{
                label: "Signal",
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
            // scales: {
            //     x: {
            //         min: 0,
            //         max: 0
            //     },
            //     y: {
            //         beginAtZero: false
            //     }
            // },
            elements: {
                point: {
                    radius: 0
                }
            }
        },
        plugins: []
    };
    var chartSignal = new Chart(ctxSignal, chart_signal_config);

    function parseValues(data){
        var values = []
        data.signal.values.forEach(element => {
            values.push(math.number(element));
        });
        return values;
    }

    function parseTimestamps(data){
        var timestamps = [];
        data.signal.timestamps.forEach(element => {
            timestamps.push(math.number(element));
        });
        return timestamps;
    }

    function updateSignalChart(chart, timestamps, values){
        var _chart_data = [];
        var display_size_ = timestamps.length;
        for(var i = 0; i < display_size_; i++){
            _chart_data.push({x: Number(timestamps[i]), y: Number(values[i])});
        }
        // chart.options.scales.x.max = timestamps[display_size_-1];
        // chart.options.scales.x.min = timestamps[0];
        // chart.options.scales.y.min = Math.min(values);
        // chart.options.scales.y.max = Math.max(values);
        chart.data.datasets[0].data = _chart_data;
        chart.update();
    }

    function plotSignal(data){
        var values = parseValues(data);
        var timestamps = parseTimestamps(data);
        updateSignalChart(chartSignal, timestamps, values);
    }

    function get_data(){
        $.ajax({
            method: 'GET',
            url: "http://127.0.0.1:8801/data",
            dataType: 'json',
            contentType: 'text/plain; charset=utf-8',
            success: (data, textStatus, jqXHR)=>{
                plotDataFFT(data);
                plotDataFFTLog(data);
                plotSignal(data);
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
