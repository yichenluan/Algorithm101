package main

import (
	"fmt"
	"sort"
)

func BubbleSort(data sort.Interface) {
	n := data.Len()
	for i := 0; i < n; i++ {
		swapped := false
		for j := 0; j < n-i-1; j++ {
			if data.Less(j+1, j) {
				data.Swap(j, j+1)
				swapped = true
			}
		}
		if !swapped {
			break
		}
	}
	return
}

func main() {
	unorder := sort.IntSlice{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
	fmt.Printf("before order: %v\n", unorder)

	BubbleSort(unorder)

	fmt.Printf("after order: %v\n", unorder)
}
