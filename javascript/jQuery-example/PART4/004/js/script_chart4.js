; (function (d, $) {

    var jQdm_graph;

    jQdm_graph = function () {

        var _root = this;

        //
        // chart1
        // 折線圖
        //

        this.chart1_setting = { // 動畫折線圖

            type: "line", // 設定圖形型態
            margins: [20, 40, 40, 40],　// [10,10,10,10] -> [50,50,50,50]　設定圖形margin
            barMargins: 50, // 橫軸單一區域內的margin，值越大則該區域的長條圖寬度越小

            labels: // 橫軸Label
            ["2013-06", "2013-07", "2013-08", "2013-09", "2013-10", "2013-11"],
            values: { // 各圖的值
                serie1: [42.68, 43.12, 42.78, 40.8, 40.44, 41.87],
                serie2: [25.44, 24.53, 25.55, 28.56, 28.96, 27.31],
                serie3: [20.01, 20.09, 19.25, 18.36, 18.11, 18.15],
                serie4: [8.39, 8.59, 8.57, 8.52, 8.54, 8.5],
                serie5: [1.03, 1.1, 1.16, 1.16, 1.2, 1.18]
            },
            series: { // 各圖的顏色
                serie1: {
                    color: "#f1c40f"
                },
                serie2: {
                    color: "#e67e22"
                },
                serie3: {
                    color: "#e74c3c"
                },
                serie4: {
                    color: "#27ae60"
                },
                serie5: {
                    color: "#2980b9"
                }
            },
            legend: { // 各圖的圖標
                serie1: 'Chrome',
                serie2: 'IE',
                serie3: 'Firefox',
                serie4: 'Safari',
                serie5: 'Opeara'
            },
            tooltips: { // 各圖的tooltip
                serie1: ["42.68%", "43.12%", "42.78%", "40.8%", "40.44%", "41.87%"],
                serie2: ["25.44%", "24.53%", "25.55%", "28.56%", "28.96%", "27.31%"],
                serie3: ["20.01%", "20.09%", "19.25%", "18.36%", "18.11%", "18.15%"],
                serie4: ["8.39%", "8.59%", "8.57%", "8.52%", "8.54%", "8.5%"],
                serie5: ["1.03%", "1.1%", "1.16%", "1.16%", "1.2%", "1.18%"]
            },

            defaultSeries: { // 圖形設定（額外部分）

                type: 'line',  //　將圖形型態設定為line(折線圖)
                plotProps: {
                    "stroke-width": 2 // 線寬
                },

                fill: true,

                startAnimation: {
                    active: true,
                    type: 'grow',
                    speed: 1200,
                    delay: 600,
                    easing: 'elastic'
                },

                dot: true,  // false 圖形是否顯示圓點
                dotProps: {
                    size: 4,
                    stroke: "#000",
                    zindex: 5
                },

                tooltip: { //　圖形tooltip的設定
                    active: true,  // true 是否支援tooltip？
                    width: 80, // tooltip寬度
                    height: 40, //　tooltip高度
                    roundedCorners: 5,  // 設定tooltip圓角（px）
                    padding: [10, 10], // [6, 6] tooltip padding，同CSS依照[縱、橫]的順序
                    offset: [10, 10], // [20, 0] tooltip margin，同CSS依照[縱、橫]的順序
                    frameProps: {  // tooltip顯示
                        fill: "rgba( 0, 0, 0, 0.7)",  // white 背景色
                        "stroke-width": 4 // 2 框線顏色
                    },
                    contentStyle: {  // tooltip的內容樣式
                        "font-family": "Arial", // font
                        "font-size": "16px",  // 12px font size
                        "line-height": "24px",  // 16px 行高
                        color: "#fff"  // black 文字顏色
                    }
                },

                highlight: { // 滑鼠移到圖形時的動作
                    scale: [1, 1], // [x,y]->[2,1] highlight時的放大率
                    scaleSpeed: 100, scaleEasing: '',
                    newProps: {
                        opacity: 1
                    },
                    overlayProps: {
                        "fill": "white",
                        "fill-opacity": .3,
                        "stroke-width": 0
                    },
                    move: 0,
                    moveSpeed: 100, moveEasing: '',
                    restoreSpeed: 0, restoreEasing: ''
                },

                legend: { // 圖標內容樣式設定
                    textProps: { // 圖標文字樣式
                        fill: "#999"
                    }
                }

            },

            defaultAxis: { // defaultAxis(軸)的設定

                max: 50, // x -> 50  値的最大値
                labels: true, // false -> true 設定是否顯示軸的label
                labelsDistance: 15, // 8 label間的距離（px）
                labelsProps: { // label樣式
                    font: '10px Arial',
                    fill: "#000"
                },
                labelsAnchor: "auto", // auto 設定軸label的anchor點(以labelDistance、labelRotate為基礎點)
                //auto:軸label的中央
                //start:軸label的右側
                //end:軸label的左側

                labelsMargin: 0, // 0 軸label的margin，設定縱軸上方margin與橫軸左側margin

            },

            features: {　// 設定部分的設定

                grid: { // grid的設定
                    nx: "auto", // auto 橫軸label顯示數
                    ny: 6, // 4 縱軸label顯示數
                    draw: true,　// false -> true 是否顯示grid
                    props: { // grid樣式
                        stroke: '#dadada', // #e0e0e0
                        "stroke-width": 1 // 1
                    },
                    extra: [0, 0, 10, 0] // [0,0,0,0]　格線的超出寬度[上, 右, 下, 左]

                },

                legend: {　// 圖標額外設定
                    x: 60, // 'auto' 圖標的x作標，當horizontal為true時，auto有效
                    y: 10,
                    width: 100, // horizontal為true時，auto有效
                    height: 100,
                    itemWidth: "fixed",
                    // fixed 圖標各項目的寬度，fixed為固定。
                    margins: [0, 0, 0, 0],
                    dotMargins: [10, 5], // sx, dx
                    borderProps: {
                        fill: "rgba( 255, 255, 255, 0.5)",
                        stroke: "black",
                        "stroke-width": 0
                    },
                    // 下面defaultSeries設定為優先
                    dotType: 'rect',
                    dotWidth: 10, dotHeight: 10, dotR: 4,
                    dotProps: {
                        type: "rect",
                        width: 10,
                        height: 10,
                        "stroke-width": 0
                    },
                    textProps: { font: '12px Arial', fill: "#000" }
                }
            },

            axis: {
                l: {
                    suffix: '%'
                }
            }

        };


        //
        // chart2
        // 長條圖
        //

        this.chart2_setting = { // 動畫長條圖

            type: "line", // 設定圖形型態
            margins: [20, 40, 40, 40],　// [10,10,10,10] -> [50,50,50,50]　グラフのmarginを指定
            barMargins: 50, // 橫軸單一區域內的margin，值越大則該區域的長條圖寬度越小

            labels: // 橫軸Label
            ["2013-06", "2013-07", "2013-08", "2013-09", "2013-10", "2013-11"],
            values: { // 各圖的值
                serie1: [42.68, 43.12, 42.78, 40.8, 40.44, 41.87],
                serie2: [25.44, 24.53, 25.55, 28.56, 28.96, 27.31],
                serie3: [20.01, 20.09, 19.25, 18.36, 18.11, 18.15],
                serie4: [8.39, 8.59, 8.57, 8.52, 8.54, 8.5],
                serie5: [1.03, 1.1, 1.16, 1.16, 1.2, 1.18]
            },
            series: { // 各圖的顏色
                serie1: {
                    color: "#f1c40f"
                },
                serie2: {
                    color: "#e67e22"
                },
                serie3: {
                    color: "#e74c3c"
                },
                serie4: {
                    color: "#27ae60"
                },
                serie5: {
                    color: "#2980b9"
                }
            },
            legend: { // 各圖的圖標
                serie1: 'Chrome',
                serie2: 'IE',
                serie3: 'Firefox',
                serie4: 'Safari',
                serie5: 'Opeara'
            },
            tooltips: { // 各圖的tooltip
                serie1: ["42.68%", "43.12%", "42.78%", "40.8%", "40.44%", "41.87%"],
                serie2: ["25.44%", "24.53%", "25.55%", "28.56%", "28.96%", "27.31%"],
                serie3: ["20.01%", "20.09%", "19.25%", "18.36%", "18.11%", "18.15%"],
                serie4: ["8.39%", "8.59%", "8.57%", "8.52%", "8.54%", "8.5%"],
                serie5: ["1.03%", "1.1%", "1.16%", "1.16%", "1.2%", "1.18%"]
            },

            defaultSeries: { // 圖形設定（額外部分）

                type: 'bar',  //　將圖形型態設定為bar(長條圖)
                barWidthPerc: 90, // 設定單一長條圖寬度的區域比例，設定100%則長條圖間無間隙
                stacked: true,
                plotProps: {
                    "stroke-width": 0 // 無框線的長條圖
                },

                startAnimation: {
                    active: true,
                    type: 'grow',
                    speed: 1200,
                    delay: 600,
                    easing: 'bounce'
                },

                dot: true,  // false 圖形是否顯示圓點
                dotProps: {
                    size: 4,
                    stroke: "#000",
                    zindex: 5
                },

                tooltip: { //　圖形tooltip的設定
                    active: true,  // true 是否支援tooltip？
                    width: 80, // tooltip寬度
                    height: 40, //　tooltip高度
                    roundedCorners: 5,  // 設定tooltip圓角（px）
                    padding: [10, 10], // [6, 6] tooltip padding，同CSS依照[縱、橫]的順序
                    offset: [10, 10], // [20, 0] tooltip margin，同CSS依照[縱、橫]的順序
                    frameProps: {  // tooltip顯示
                        fill: "rgba( 0, 0, 0, 0.7)",  // white 背景色
                        "stroke-width": 4 // 2 框線顏色
                    },
                    contentStyle: {  // tooltip的內容樣式
                        "font-family": "Arial", // font
                        "font-size": "16px",  // 12px font size
                        "line-height": "24px",  // 16px 行高
                        color: "#fff"  // black 文字顏色
                    }
                },

                highlight: { // 滑鼠移到圖形時的動作
                    scale: [1, 1], // [x,y]->[2,1] highlight時的放大率
                    scaleSpeed: 100, scaleEasing: '',
                    newProps: {
                        opacity: 1
                    },
                    overlayProps: {
                        "fill": "white",
                        "fill-opacity": .3,
                        "stroke-width": 0
                    },
                    move: 0,
                    moveSpeed: 100, moveEasing: '',
                    restoreSpeed: 0, restoreEasing: ''
                },

                legend: { // 圖標內容樣式設定
                    textProps: { // 圖標文字樣式
                        fill: "#999"
                    }
                }

            },

            defaultAxis: { //  defaultAxis(軸)的設定

                max: 50, // x -> 50  値的最大値
                labels: true, // false -> true 設定是否顯示軸的label
                labelsDistance: 15, // 8 label間的距離（px）
                labelsProps: { // label樣式
                    font: '10px Arial',
                    fill: "#000"
                },
                labelsAnchor: "auto", // auto 設定軸label的anchor點(以labelDistance、labelRotate為基礎點)
                //auto:軸label的中央
                //start:軸label的右側
                //end:軸label的左側

                labelsMargin: 0, // 0 軸label的margin，設定縱軸上方margin與橫軸左側margin

            },

            features: {　// 設定部分的設定

                grid: { // grid的設定
                    nx: "auto", // auto 橫軸label顯示數
                    ny: 6, // 4 縱軸label顯示數
                    draw: true,　// false -> true 是否顯示grid
                    props: { // grid樣式
                        stroke: '#dadada', // #e0e0e0
                        "stroke-width": 1 // 1
                    },
                    extra: [0, 0, 10, 0] // [0,0,0,0]　格線的超出寬度[上, 右, 下, 左]

                },

                legend: {　// 注釈の追加設定
                    x: 60, // 'auto' 圖標的x作標，當horizontal為true時，auto有效
                    y: 10,
                    width: 100, // horizontal為true時，auto有效
                    height: 100,
                    itemWidth: "fixed",
                    // fixed 圖標各項目的寬度，fixed為固定。
                    margins: [0, 0, 0, 0],
                    dotMargins: [10, 5], // sx, dx
                    borderProps: {
                        fill: "rgba( 255, 255, 255, 0.5)",
                        stroke: "black",
                        "stroke-width": 0
                    },
                    // 下面defaultSeries設定為優先
                    dotType: 'rect',
                    dotWidth: 10, dotHeight: 10, dotR: 4,
                    dotProps: {
                        type: "rect",
                        width: 10,
                        height: 10,
                        "stroke-width": 0
                    },
                    textProps: { font: '12px Arial', fill: "#000" }
                }
            },

            axis: {
                l: {
                    max: 120,
                    suffix: '%'
                }
            }

        };


        //
        // chart3
        // 圓餅圖
        //

        this.chart3_setting = { // 動畫圓餅圖

            type: "pie", //　設定圖形型態
            margins: [10, 10, 10, 10],　// [10,10,10,10]　設定圖形margin

            labels: [ // 橫軸Label
            'Chrome', 'IE', 'Firefox', 'Safari', 'Opeara', 'Etc'
            ],

            values: { //各圖的值
                serie1: [42.68, 25.44, 20.01, 8.39, 1.03, 2.45]
            },

            legend: [ // 各圖的圖標
            'Chrome', 'IE', 'Firefox', 'Safari', 'Opeara', 'Etc'
            ],

            tooltips: { // 各圖的tooltip
                serie1: ["Chrome 42.68%", "IE 25.44%", "Firefox 20.01%", "Safari 8.39%", "Opeara 1.03%", "Etc 2.45%"]
            },

            startAngle: 270, // 0 圓餅圖開始繪製的角度
            clockwise: true, // false 是否順時針
            valueThresold: 0.006, // 0.006

            defaultSeries: { // 設定圓餅圖

                r: 1, // 1 圓餅圖大小比例。設定1~-1，-1為甜甜圈形狀

                plotProps: {
                    "stroke-width": 0,
                    opacity: 1
                },

                values: [ // 各圖形設定
                  {
                      plotProps: {
                          fill: "#f1c40f"
                      }
                  },
                  {
                      plotProps: {
                          fill: "#e67e22"
                      }
                  },
                  {
                      plotProps: {
                          fill: "#e74c3c"
                      }
                  },
                  {
                      plotProps: {
                          fill: "#27ae60"
                      }
                  },
                  {
                      plotProps: {
                          fill: "#2980b9"
                      },
                      label: { // 讓label清楚顯示，所以調整其預設位置
                          offset: [80, 0]
                      }
                  },
                  {
                      plotProps: {
                          fill: "#ccc"
                      },
                      label: {　// 讓label清楚顯示，所以調整其預設位置
                          offset: [40, 0]
                      }

                  }
                ],

                label: { // 於圖上顯示label
                    active: true,
                    offset: [40, 10],
                    props: {
                        fill: "#000"
                    }
                },

                highlight: { // 滑鼠移入時的動作
                    move: 10,
                    newProps: {
                    },
                    overlayProps: {
                        "fill": "#fff",
                        "fill-opacity": .3,
                        "stroke-width": 0
                    }
                },

                tooltip: { //　圖形tooltip的設定
                    active: true,  // true 是否支援tooltip？
                    width: 160, // tooltip寬度
                    height: 40,  //　tooltip高度
                    roundedCorners: 5,  // 設定tooltip圓角（px）
                    padding: [10, 10], // [6, 6] tooltip padding，同CSS依照[縱、橫]的順序
                    offset: [10, 10], // [20, 0] tooltip margin，同CSS依照[縱、橫]的順序
                    frameProps: {  // tooltip顯示
                        fill: "rgba( 0, 0, 0, 0.7)",  // white 背景色
                        "stroke-width": 4 // 2 框線顏色
                    },
                    contentStyle: {  // tooltip的內容樣式
                        "font-family": "Arial", // font
                        "font-size": "16px",  // 12px font size
                        "line-height": "24px",  // 16px 行高
                        "text-align": "center",
                        color: "#fff"  // black 文字顏色
                    }
                },

                startAnimation: {
                    active: true,
                    type: "avg",
                    speed: 1200,
                    delay: 0,
                    easing: ''
                }

            },

            features: {　// 設定部分的設定

                animation: {
                    active: true,
                    propsFrom: {
                        "opacity": 0
                    },
                    propsTo: {
                        "opacity": 1
                    },
                    speed: 600,
                    delay: 1000
                },


                legend: {　// 圖標額外設定
                    x: 60, // 'auto' 圖標的x作標，當horizontal為true時，auto有效
                    y: 10,
                    width: 100, // horizontal為true時，auto有效
                    height: 100,
                    itemWidth: "fixed",
                    // fixed 圖標各項目的寬度，fixed為固定。
                    margins: [0, 0, 0, 0],
                    dotMargins: [10, 5], // sx, dx
                    borderProps: {
                        fill: "rgba( 255, 255, 255, 0.5)",
                        stroke: "black",
                        "stroke-width": 0
                    },
                    // 下面defaultSeries設定為優先
                    dotType: 'rect',
                    dotWidth: 10, dotHeight: 10, dotR: 4,
                    dotProps: {
                        type: "rect",
                        width: 10,
                        height: 10,
                        "stroke-width": 0
                    },
                    textProps: { font: '12px Arial', fill: "#000" }
                }
            }

        };

        $('.showChart a').on('click', function (e) {
            if (typeof e.preventDefault() !== undefined) {
                e.preventDefault();
            }
            var _t = $(this).attr('href');
            if (_t) {
                $(_t).hide().chart('clear').fadeIn(500).chart(_root[replaceString(_t) + '_setting']);
            }
        });


        // 文字列置換処理
        function replaceString(_str, _bf, _af, _flg) {
            var _reg = new RegExp(_bf || '[\\.#]', _flg || '');
            return _str ? _str.replace(_reg, _af || '') : false;
        }

    }

    new jQdm_graph();

})(document, jQuery);