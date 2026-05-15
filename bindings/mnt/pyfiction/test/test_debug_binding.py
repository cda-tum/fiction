def test_check_bindings():
    print("\nDEBUG: Attempting to import mnt.pyfiction")
    try:
        from mnt import pyfiction

        print("DEBUG: Successfully imported mnt.pyfiction")
        print("DEBUG: dir(pyfiction):", dir(pyfiction))
        if hasattr(pyfiction, "get_sidb_charge_states_base_2"):
            print("DEBUG: get_sidb_charge_states_base_2 IS PRESENT")
            # Call it to ensure it works
            states = pyfiction.get_sidb_charge_states_base_2()
            print(f"DEBUG: get_sidb_charge_states_base_2 returned: {states}")
        else:
            print("DEBUG: get_sidb_charge_states_base_2 IS MISSING")

        # Check if sidb_charge_state is present
        if hasattr(pyfiction, "sidb_charge_state"):
            print("DEBUG: sidb_charge_state enum IS PRESENT")
        else:
            print("DEBUG: sidb_charge_state enum IS MISSING")

    except Exception as e:
        print(f"DEBUG: Import failed with {type(e).__name__}: {e}")
        raise
