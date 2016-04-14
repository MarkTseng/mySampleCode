; (function (d, $) {

    var jQdm_graph;

    jQdm_graph = function () {

        //
        // sample3_1
        // 繪製Label、圖標、tooltip的圓餅圖
        //

        var sample3_1_setting = { // Sample3的選項設定

            type: "pie", //　設定圖形型態為圓餅圖
            margins: [10, 10, 10, 10],　// [10,10,10,10]　設定圖形margin

            labels: [ // 各圖形的label
        'Chrome', 'IE', 'Firefox', 'Safari', 'Opeara', 'Etc'
            ],

            values: { // 各圖形的值
                serie1: [42.68, 25.44, 20.01, 8.39, 1.03, 2.45]
            },

            legend: [ // 各圖形的圖標
        'Chrome', 'IE', 'Firefox', 'Safari', 'Opeara', 'Etc'
            ],

            tooltips: { // 各圖形的tooltip
                serie1: ["Chrome 42.68%", "IE 25.44%", "Firefox 20.01%", "Safari 8.39%", "Opeara 1.03%", "Etc 2.45%"]
            },

            startAngle: 270, // 0 圓餅圖開始繪製的角度
            clockwise: true, // false 是否順時針

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
                }

            },

            features: {

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

        // 繪製sample3_1的圖
        $("#chart3_1").chart(sample3_1_setting);

        // 將sample3_1的圖製作樣版
        $.elycharts.templates['template_sample3_1'] = sample3_1_setting;

        //
        // sample3_2
        // 將第1個範例做成甜甜圈形狀
        //

        // 繪製sample3_2的圖
        $("#chart3_2").chart({
            template: 'template_sample3_1',
            defaultSeries: {
                r: -.5,
                label: {
                    offset: [0, 0]
                },
                values: [
                  {},
                  {},
                  {},
                  {},
                  {
                      label: { // 讓label清楚顯示，所以調整其預設位置
                          offset: [15, 0]
                      }
                  },
                  {}
                ],
            }
        });




    }

    jQdm_graph();

})(document, jQuery);