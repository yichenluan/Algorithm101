package main

import (
	"fmt"
	"sort"
)

func SelectionSort(data sort.Interface) {
	n := data.Len()
	for i := 0; i < n-1; i++ {
		minIdx := i
		for j := i; j < n; j++ {
			if data.Less(j, minIdx) {
				minIdx = j
			}
		}
		data.Swap(minIdx, i)
	}
}

func main() {
	unorder := sort.IntSlice{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
	fmt.Printf("before order: %v\n", unorder)

	SelectionSort(unorder)

	fmt.Printf("after order: %v\n", unorder)
}
