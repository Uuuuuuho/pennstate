#!/usr/bin/env python3
"""
Parse and display InactiveCodeTracker annotations from LLVM IR
"""
import re
import sys

def parse_annotations(ir_file):
    """Extract and display inactive block annotations"""
    with open(ir_file, 'r') as f:
        content = f.read()
    
    # Find all annotation strings
    pattern = r'c"(inactive_block:[^"]*)"'
    matches = re.findall(pattern, content)
    
    print("=" * 100)
    print("INACTIVE CODE BLOCKS DETECTED IN LLVM IR")
    print("=" * 100)
    
    for idx, annotation in enumerate(matches, 1):
        print(f"\n[Block #{idx}]")
        print("-" * 100)
        
        # Parse the annotation string
        parts = annotation.split()
        
        # Extract location
        location = parts[1]  # e.g., /path/file.c:36:1-42:7
        print(f"Location: {location}")
        
        # Extract condition if present
        for part in parts:
            if part.startswith('condition='):
                print(f"Condition: {part.split('=')[1]}")
        
        # Extract code content
        for part in parts:
            if part.startswith('code='):
                code_part = part[5:]  # Remove 'code='
                # Replace underscores back to spaces and newlines
                code_display = code_part.replace('_', ' ')
                print(f"Code Content: {code_display}")
                break
        
        print()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: parse_annotations.py <llvm_ir_file>")
        sys.exit(1)
    
    parse_annotations(sys.argv[1])
