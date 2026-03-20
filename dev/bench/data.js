window.BENCHMARK_DATA = {
  "lastUpdate": 1773976493043,
  "repoUrl": "https://github.com/nevergiveupcpp/obfuscxx",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "217945237+nevergiveupcpp@users.noreply.github.com",
            "name": "Alexander",
            "username": "nevergiveupcpp"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9c60308cc3b83b24529eefae6dc4b7b183d6a826",
          "message": "Merge pull request #5 from nevergiveupcpp/test\n\nci: retrigger benchmark",
          "timestamp": "2026-03-20T10:11:55+07:00",
          "tree_id": "3b6d592c19149e02414cc587a127409874a8c385",
          "url": "https://github.com/nevergiveupcpp/obfuscxx/commit/9c60308cc3b83b24529eefae6dc4b7b183d6a826"
        },
        "date": 1773976490740,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_IntegerGet_Low",
            "value": 3.9911768973212505,
            "unit": "ns/iter",
            "extra": "iterations: 179200000\ncpu: 3.8364955357142856 ns\nthreads: 1"
          },
          {
            "name": "BM_IntegerGet_Medium",
            "value": 13.403535208019388,
            "unit": "ns/iter",
            "extra": "iterations: 49777778\ncpu: 13.18359369114467 ns\nthreads: 1"
          },
          {
            "name": "BM_IntegerGet_High",
            "value": 38.28194931639403,
            "unit": "ns/iter",
            "extra": "iterations: 18666667\ncpu: 38.50446359813458 ns\nthreads: 1"
          },
          {
            "name": "BM_FloatGet_Low",
            "value": 3.7228494576379227,
            "unit": "ns/iter",
            "extra": "iterations: 186666667\ncpu: 3.6830357077088647 ns\nthreads: 1"
          },
          {
            "name": "BM_FloatGet_Medium",
            "value": 13.370466234953547,
            "unit": "ns/iter",
            "extra": "iterations: 49777778\ncpu: 13.18359369114467 ns\nthreads: 1"
          },
          {
            "name": "BM_FloatGet_High",
            "value": 38.145835033112334,
            "unit": "ns/iter",
            "extra": "iterations: 18666667\ncpu: 38.50446359813458 ns\nthreads: 1"
          },
          {
            "name": "BM_StringGet_Low",
            "value": 16.363494310066415,
            "unit": "ns/iter",
            "extra": "iterations: 40727273\ncpu: 16.113281142098565 ns\nthreads: 1"
          },
          {
            "name": "BM_StringGet_Medium",
            "value": 34.41061311447632,
            "unit": "ns/iter",
            "extra": "iterations: 20363636\ncpu: 32.22656307547434 ns\nthreads: 1"
          },
          {
            "name": "BM_StringGet_High",
            "value": 83.8468221496976,
            "unit": "ns/iter",
            "extra": "iterations: 7466667\ncpu: 83.70535340601101 ns\nthreads: 1"
          },
          {
            "name": "BM_WStringGet_Low",
            "value": 16.727687611198487,
            "unit": "ns/iter",
            "extra": "iterations: 40727273\ncpu: 16.49693069310091 ns\nthreads: 1"
          },
          {
            "name": "BM_WStringGet_Medium",
            "value": 34.70690106619579,
            "unit": "ns/iter",
            "extra": "iterations: 20363636\ncpu: 34.528460438008224 ns\nthreads: 1"
          },
          {
            "name": "BM_WStringGet_High",
            "value": 84.04953125000146,
            "unit": "ns/iter",
            "extra": "iterations: 8960000\ncpu: 85.44921875 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayIteration_Low",
            "value": 365.4282261351743,
            "unit": "ns/iter",
            "extra": "iterations: 1947826\ncpu: 369.0011325446934 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayIteration_Medium",
            "value": 1342.473351574356,
            "unit": "ns/iter",
            "extra": "iterations: 497778\ncpu: 1286.9692915315663 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayIteration_High",
            "value": 3822.789620535616,
            "unit": "ns/iter",
            "extra": "iterations: 179200\ncpu: 3749.3024553571427 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayCopyTo_Low",
            "value": 122.88032142857372,
            "unit": "ns/iter",
            "extra": "iterations: 5600000\ncpu: 122.76785714285714 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayCopyTo_Medium",
            "value": 245.5029285714261,
            "unit": "ns/iter",
            "extra": "iterations: 2800000\ncpu: 245.53571428571428 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayCopyTo_High",
            "value": 554.8949107142574,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 544.0848214285714 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayGet_Low",
            "value": 3.71165838622927,
            "unit": "ns/iter",
            "extra": "iterations: 186666667\ncpu: 3.6830357077088647 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayGet_Medium",
            "value": 13.373403288512215,
            "unit": "ns/iter",
            "extra": "iterations: 49777778\ncpu: 13.18359369114467 ns\nthreads: 1"
          },
          {
            "name": "BM_ArrayGet_High",
            "value": 38.25966360250468,
            "unit": "ns/iter",
            "extra": "iterations: 18666667\ncpu: 37.66741004165339 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}