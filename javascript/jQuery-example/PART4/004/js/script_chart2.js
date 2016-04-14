; (function (d, $) {

    var jQdm_graph;

    jQdm_graph = function () {

        //
        // sample2_1
        // 於條狀網格背景上繪製有圖標、tooltip的長條圖
        //

        var sample2_1_setting = { // Sample2的基礎參數(大致與Sample1_3相同)

            type: "line", // 設定圖形型態
            margins: [20, 40, 40, 40],　// 設定圖形margin

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

                fill: false,
                fillProps: { // 填滿的樣式
                    stroke: "none",
                    "stroke-width": 0,
                    "stroke-opacity": 0,
                    opacity: .5
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
                    scale: [1, 1], // highlight時的放大率
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

                plotProps: { // 圖形外觀設定
                    "stroke-width": 2 //　line圖線寬
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

        // 將sample2_1的圖製作成樣版
        $.elycharts.templates['template_sample2_1'] = sample2_1_setting;

        // 繪製sample2_1的圖
        $("#chart2_1").chart({
            template: 'template_sample2_1',
            type: "line", // 設定圖形型態
            barMargins: 50, // 橫軸單一區域內的margin，值越大則該區域的長條圖寬度越小
            defaultSeries: {

                type: 'bar',  //　將圖形型態設定為bar(長條圖)
                barWidthPerc: 90, // 長條圖固有屬性，設定長條圖寬度(%)，設定最大值100%則長條圖間無間隙
                plotProps: {
                    "stroke-width": 0 // 設定為0，表示不顯示外框
                }
            },

            features: {
                grid: {
                    evenHProps: { // 設定grid y軸偶數區域的背景色為灰色
                        fill: "#ccc",
                        opacity: 0.2,
                        stroke: '#e0e0e0',
                        "stroke-width": 1
                    },
                    oddHProps: {　// 設定grid y軸奇數區域的背景色為亮色
                        fill: "#eee",
                        opacity: 0.2,
                        stroke: '#e0e0e0',
                        "stroke-width": 1
                    }
                }
            }

        });



        //
        // sample2_2
        // 更改為累積型長條圖
        //

        // 繪製sample2_2的圖
        $("#chart2_2").chart({
            template: 'template_sample2_1',
            type: "line", // 設定圖形型態

            values: { // 各圖的值
                serie6: [42.68, 43.12, 42.78, 40.8, 40.44, 41.87],
                serie7: [25.44, 24.53, 25.55, 28.56, 28.96, 27.31],
                serie8: [20.01, 20.09, 19.25, 18.36, 18.11, 18.15],
                serie9: [8.39, 8.59, 8.57, 8.52, 8.54, 8.5],
                serie10: [1.03, 1.1, 1.16, 1.16, 1.2, 1.18]
            },
            series: { 
                serie6: {
                    type: "line",
                    plotProps: {
                        "stroke-width": 1
                    },
                    color: "#f1c40f"
                },
                serie7: {
                    type: "line",
                    plotProps: {
                        "stroke-width": 1
                    },
                    color: "#e67e22"
                },
                serie8: {
                    type: "line",
                    plotProps: {
                        "stroke-width": 1
                    },
                    color: "#e74c3c"
                },
                serie9: {
                    type: "line",
                    plotProps: {
                        "stroke-width": 1
                    },
                    color: "#27ae60"
                },
                serie10: {
                    type: "line",
                    plotProps: {
                        "stroke-width": 1
                    },
                    color: "#2980b9"
                }
            },

            tooltips: { // 各圖的tooltip
                serie6: ["42.68%", "43.12%", "42.78%", "40.8%", "40.44%", "41.87%"],
                serie7: ["25.44%", "24.53%", "25.55%", "28.56%", "28.96%", "27.31%"],
                serie8: ["20.01%", "20.09%", "19.25%", "18.36%", "18.11%", "18.15%"],
                serie9: ["8.39%", "8.59%", "8.57%", "8.52%", "8.54%", "8.5%"],
                serie10: ["1.03%", "1.1%", "1.16%", "1.16%", "1.2%", "1.18%"]
            },

            barMargins: 60, // 橫軸單一區域內的margin，值越大則該區域的長條圖寬度越小
            defaultSeries: {

                type: 'bar',  //　將圖形型態設定為bar(長條圖)
                barWidthPerc: 90, // 長條圖固有屬性，設定長條圖寬度(%)，設定最大值100%則長條圖間無間隙
                plotProps: {
                    "stroke-width": 0 // 設定為0，表示不顯示外框
                },
                stacked: true

            },

            defaultAxis: {
                max: 120,
            },

            features: {
                grid: {
                    evenHProps: { // 設定grid y軸偶數區域的背景色為灰色
                        fill: "#ccc",
                        opacity: 0.2,
                        stroke: '#e0e0e0',
                        "stroke-width": 1
                    },
                    oddHProps: {　// 設定grid y軸奇數區域的背景色為亮色
                        fill: "#eee",
                        opacity: 0.2,
                        stroke: '#e0e0e0',
                        "stroke-width": 1
                    }
                },

                legend: {
                    horizontal: true,
                    width: 'auto',
                    height: 20,
                    y: 40,
                    itemWidth: "auto"
                }

            }

        });



    }

    jQdm_graph();

})(document, jQuery);