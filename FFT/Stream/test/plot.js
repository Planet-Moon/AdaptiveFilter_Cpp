$(()=>{
    const ctx = $('#myChart');

    var chart_data; // = [{x:0, y:0},{x:1, y:1},{x:2, y:2}];

    var chart_config = {
        type: 'scatter',
        data: {
            datasets: [{
                label: "FFT",
                data: chart_data,
                showLine: true,
                tension: 0
            }]
        },
        options: {
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

    var chartLog = new Chart(ctx, chart_config);

    function parseFrequencies(data){
        var frequencies = [];
        data.freq.forEach(element => {
            frequencies.push(math.number(element));
        });
        return frequencies;
    }

    function paresMagnitudeLog(data) {
        var magnitudes = [];
        data.fft.forEach(element => {
            var cmplx = math.complex(element);
            var value = 20*math.log10(math.sqrt(math.pow(cmplx.re, 2) + math.pow(cmplx.im, 2)));
            magnitudes.push(value);
        });
        return magnitudes;
    }

    function paresMagnitude(data) {
        var magnitudes = [];
        data.fft.forEach(element => {
            var cmplx = math.complex(element);
            var value = math.sqrt(math.pow(cmplx.re, 2) + math.pow(cmplx.im, 2));
            magnitudes.push(value);
        });
        return magnitudes;
    }

    function updateFFTChart(chart, magnitudes, frequencies){
        chart_data = [];
        var display_size_ = magnitudes.length/2;
        for(var i = 0; i < display_size_; i++) {
            chart_data.push({x: Number(frequencies[i]), y: Number(magnitudes[i])});
        }
        console.log(chart_data);
        chart.options.scales.x.max = display_size_;
        chart.data.datasets[0].data = chart_data;
        $("#max_freq").html(frequencies[frequencies.length-1].toFixed(2));
        chart.update();
    }

    function plotDataLog(data){
        magnitudes = paresMagnitudeLog(data);
        frequencies = parseFrequencies(data);
        updateFFTChart(chartLog, magnitudes, frequencies);
    }

    function plotData(data){
        magnitudes = paresMagnitude(data);
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
                plotDataLog(data);
                plotData(data);
            }
        });
    }

    get_data();

    setInterval(()=>{
        get_data();
    },1000);

});
